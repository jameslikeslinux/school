package org.thestaticvoid.msync;

import java.io.*;
import java.net.*;
import java.util.*;

/**
 * Main class; handles all networking logic.
 * 
 * @author James Lee
 * @version 20070509
 */
public class Msync {
	private static final int PORT = 4446;
	private String dir, hostname;
	private InetAddress group;
	private MulticastSocket socket;
	private Map peers, fileWriters;
	private FileInfo[] filesInfo;
	private FileSender fileSender;
	
	/**
	 * When told, sends files to peers if at least one doesn't have the file.
	 * 
	 * @author James Lee
	 * @version 20070509
	 */
	private class FileSender extends Thread {
		public void run() {
			while (true) {
				try {
					synchronized (this) {
						wait();		// sleep until notified
					}	
					
					sleep(5000);	// then sleep five seconds more
					
					// Read all peers' file lists to see if I have files they don't.
					for (int i = 0; i < filesInfo.length; i++) {
						boolean allHaveFile = true;
						Iterator it = peers.values().iterator();
						while (it.hasNext() && allHaveFile) {
							Peer peer = (Peer) it.next();
							FileInfo[] peersFilesInfo = peer.getFilesInfo();
							boolean peerHasFile = false;
							for (int j = 0; j < peersFilesInfo.length && !peerHasFile; j++)
								if (peersFilesInfo[j].getMd5() == filesInfo[i].getMd5())
									peerHasFile = true;
							
							if (!peerHasFile)
								allHaveFile = false;
						}
						
						// If at least one doesn't have the file, send it to the group
						if (!allHaveFile) {
							try {
								System.out.println("Sending " + filesInfo[i].getName());
								InputStream input = new FileInputStream(dir + File.separator + filesInfo[i].getName());
								byte[] buf = new byte[1024];
								int read;
								
								// Read and send byte by byte, encoded so it can be parsed on the other side.
								for (int j = 0; (read = input.read(buf)) > -1; j++) {
									byte[] message = ("DATA " + filesInfo[i].getId() + " " + j + " " + Base64.encodeBytes(buf, 0, read)).getBytes();
									DatagramPacket packet = new DatagramPacket(message, message.length, group, PORT);
									socket.send(packet);
									sleep(100);	// Need to slow down the transfer to avoid overloading the router
								}
							} catch (IOException io) {
								io.printStackTrace();
							}
						}
					}
				} catch (InterruptedException ie) {
					ie.printStackTrace();
				}
			}
		}
	}
	
	public Msync(String dir) throws IOException {
		this.dir = dir;
		hostname = InetAddress.getLocalHost().getHostName();
		peers = new LinkedHashMap();
		fileWriters = new LinkedHashMap();
		
		// Magic to tell other peers that you are leaving when shutdown.
		Runtime.getRuntime().addShutdownHook(new Thread() {
			public void run() {
				try {
					leaveGroup();
				} catch (IOException io) {
					io.printStackTrace();
				}
			}
		});
		
		fileSender = new FileSender();
		fileSender.start();
		
		// Anonymous thread to regenerate file list every five minutes
		new Thread() {
			public void run() {
				try {
					sleep(300000);	// 5 minutes
					
					generateFileList();
					sendFileList();
					
					synchronized (fileSender) {
						fileSender.notifyAll();
					}
				} catch (InterruptedException ie) {
					ie.printStackTrace();
				} catch (IOException io) {
					io.printStackTrace();
				}
			}
		}.start();
		
		// Main program logic, divided into functions.
		// These should probably stay called in this order.
		generateFileList();
		joinMulticastGroup();
		sendIdentification();
		sendFileList();
		listenToSocket();
	}
	
	// Send group "BYE"
	private void leaveGroup() throws IOException {
		System.out.println("Leaving multicast group");
		byte[] bye = "BYE".getBytes();
		DatagramPacket packet = new DatagramPacket(bye, bye.length, group, PORT);
		socket.send(packet);
		socket.leaveGroup(group);
		socket.close();
	}
	
	// Read files for a directory
	private void generateFileList() {
		System.out.println("Generating file list...");
		
		List fileInfoList = new ArrayList();
		File[] files = new File(dir).listFiles();
		
		for (int i = 0; i < files.length; i++)
			if (files[i].isFile() && files[i].canRead()) {
				System.out.println("Reading: " + files[i].getName());
				fileInfoList.add(new FileInfo(i, files[i].getName(), files[i].length(), Utils.md5(files[i])));
			}
		
		this.filesInfo = (FileInfo[]) fileInfoList.toArray(new FileInfo[]{});
	}
	
	// Just the standard stuff.
	private void joinMulticastGroup() throws IOException {
		System.out.println("Joining multicast group");
		socket = new MulticastSocket(PORT);
		group = InetAddress.getByName("230.0.0.1");
		socket.joinGroup(group);
	}
	
	// Sends "HOST <hostname>" to the group
	private void sendIdentification() throws IOException {
		byte[] id = ("HOST " + hostname).getBytes();
		DatagramPacket packet = new DatagramPacket(id, id.length, group, PORT);
		socket.send(packet);
	}
	
	// For each file list, send "FILE <id> <name> <size> <md5>" to group
	private void sendFileList() throws IOException {
		for (int i = 0; i < filesInfo.length; i++) {
			byte[] file = filesInfo[i].toString().getBytes();
			DatagramPacket packet = new DatagramPacket(file, file.length, group, PORT);
			socket.send(packet);
		}
	}
	
	// This should run forever.
	private void listenToSocket() throws IOException {		
		while (true) {
			DatagramPacket packet = new DatagramPacket(new byte[2048], 2048);	// Packets should always be less than 2 KB
			
			try {
				socket.receive(packet);
			} catch (SocketException e) {
				return;	// This happens when ^C
			}
			
			String message = new String(packet.getData()).trim();	// Get rid of null bytes!
			
			if (message.startsWith("HOST")) {
				Peer peer = Peer.parse(message);
				if (!peer.getHostname().equals(hostname) && !peers.containsKey(packet.getAddress())) {
					System.out.println("Discovered new member: " + peer.getHostname());
					peers.put(packet.getAddress(), peer);
					sendIdentification();	// Say hello to new peer
					sendFileList();
				}
			} else if (message.startsWith("FILE")) {
				Object peerObj = peers.get(packet.getAddress());
				if (peerObj != null) {
					Peer peer = (Peer) peerObj;
					FileInfo fileInfo = FileInfo.parse(message);
					System.out.println(peer.getHostname() + " has " + fileInfo.getName());
					peer.addFileInfo(fileInfo);
					
					synchronized (fileSender) {
						fileSender.notifyAll();	// Start sending files
					}
				}
			} else if (message.startsWith("DATA")) {
				Object peerObj = peers.get(packet.getAddress());
				if (peerObj != null) {
					Peer peer = (Peer) peerObj;
					String[] tokens = message.split(" ");
					if (tokens.length == 4)
						try {
							int id, kb;
							byte[] data;
							
							id = Integer.parseInt(tokens[1]);
							kb = Integer.parseInt(tokens[2]);
							
							// Now you see why I need to encode the data: so it could be tokenized.
							data = Base64.decode(tokens[3]);
							
							FileInfo fileInfo = peer.getFileInfoWithId(id);
							boolean haveFile = false;
							for (int i = 0; i < filesInfo.length && !haveFile; i++)
								if (filesInfo[i].getMd5().equals(fileInfo.getMd5()))
									haveFile = true;
							
							// Ignore the data if I already have the file.
							if (!haveFile) {
								System.out.println("Receiving kb " + kb + " of " + (fileInfo.getSize() / 1024) + " of " + fileInfo.getName());
								
								// Create or use existing FileWriter
								Object fileWriterObj = fileWriters.get(fileInfo);
								FileWriter fileWriter;
								if (fileWriterObj != null)
									fileWriter = (FileWriter) fileWriterObj;
								else {
									System.out.println("Receiving " + fileInfo.getName() + "...");
									
									if (new File(dir + File.separator + fileInfo.getName()).exists())
										fileInfo.setName(fileInfo.getName() + "." + peer.getHostname());
									
									fileWriter = new FileWriter(dir, fileInfo);
									fileWriters.put(fileInfo, fileWriter);
								}
								
								fileWriter.write(kb, data);
								
								if (fileWriter.isComplete()) {
									System.out.println("Done receiving " + fileInfo.getName());
									
									fileWriter.close();
									fileWriters.remove(fileInfo);
									generateFileList();
									sendFileList();
								}
							}
						} catch (Exception e) {
							e.printStackTrace();
						}
				}
			} else if (message.startsWith("BYE")) {
				Object peerObj = peers.get(packet.getAddress());
				if (peerObj != null) {
					System.out.println("Disconnected from group: " + ((Peer) peerObj).getHostname());
					peers.remove(packet.getAddress());
				}
			}
		}
	}
	
	public static void main(String[] args) throws IOException {		
		new Msync("/home/jlee/Backup");
	}
}
