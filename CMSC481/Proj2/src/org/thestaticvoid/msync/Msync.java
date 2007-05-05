package org.thestaticvoid.msync;

import java.io.*;
import java.net.*;
import java.util.*;

public class Msync {
	private static final int PORT = 4446;
	private String hostname;
	private InetAddress group;
	private MulticastSocket socket;
	private Map peers;
	private FileInfo[] filesInfo;
	
	public Msync() throws IOException {
		hostname = InetAddress.getLocalHost().getHostName();
		peers = new LinkedHashMap();
		
		Runtime.getRuntime().addShutdownHook(new Thread() {
			public void run() {
				try {
					leaveGroup();
				} catch (IOException io) {
					io.printStackTrace();
				}
			}
		});
		
		generateFileList();
		joinMulticastGroup();
		sendIdentification();
		sendFileList();
		listenToSocket();
	}
	
	private void leaveGroup() throws IOException {
		System.out.println("Leaving multicast group");
		byte[] bye = "BYE".getBytes();
		DatagramPacket packet = new DatagramPacket(bye, bye.length, group, PORT);
		socket.send(packet);
		socket.leaveGroup(group);
		socket.close();
	}
	
	private void generateFileList() {
		System.out.println("Generating file list...");
		
		List fileInfoList = new ArrayList();
		File[] files = new File("/home/jlee/Backup").listFiles();
		
		for (int i = 0; i < files.length; i++)
			if (files[i].isFile() && files[i].canRead()) {
				System.out.println("Reading: " + files[i].getName());
				fileInfoList.add(new FileInfo(files[i].getName(), files[i].length(), Utils.md5(files[i])));
			}
		
		this.filesInfo = (FileInfo[]) fileInfoList.toArray(new FileInfo[]{});
	}
	
	private void joinMulticastGroup() throws IOException {
		System.out.println("Joining multicast group");
		socket = new MulticastSocket(PORT);
		group = InetAddress.getByName("230.0.0.1");
		socket.joinGroup(group);
	}
	
	private void sendIdentification() throws IOException {
		byte[] id = ("HOST " + hostname).getBytes();
		DatagramPacket packet = new DatagramPacket(id, id.length, group, PORT);
		socket.send(packet);
	}
	
	private void sendFileList() throws IOException {
		for (int i = 0; i < filesInfo.length; i++) {
			byte[] file = filesInfo[i].toString().getBytes();
			DatagramPacket packet = new DatagramPacket(file, file.length, group, PORT);
			socket.send(packet);
		}
	}
	
	private void listenToSocket() throws IOException {		
		while (true) {
			DatagramPacket packet = new DatagramPacket(new byte[2048], 2048);
			
			try {
				socket.receive(packet);
			} catch (SocketException e) {
				return;
			}
			
			String message = new String(packet.getData()).trim();
			if (message.startsWith("HOST")) {
				Peer peer = Peer.parse(message);
				if (!peer.getHostname().equals(hostname) && !peers.containsKey(packet.getAddress())) {
					System.out.println("Discovered new member: " + peer.getHostname());
					peers.put(packet.getAddress(), peer);
					sendIdentification();
					sendFileList();
				}
			} else if (message.startsWith("FILE")) {
				Object peerObj = peers.get(packet.getAddress());
				if (peerObj != null) {
					Peer peer = (Peer) peerObj;
					FileInfo fileInfo = FileInfo.parse(message);
					System.out.println(peer.getHostname() + " has " + fileInfo.getName());
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
		new Msync();
	}
}
