package org.thestaticvoid.msync;

import java.util.*;

/**
 * Contains information about peers.
 * 
 * @author James Lee
 * @version 20070509
 */
public class Peer {
	private String hostname;
	private Set filesInfo;
	
	private Peer(String hostname) {
		this.hostname = hostname;
		filesInfo = new HashSet();
	}
	
	public static Peer parse(String string) {
		String[] tokens = string.split(" ");
		
		if (tokens.length == 2 && tokens[0].equals("HOST"))
			return new Peer(tokens[1]);
		
		return null;
	}
	
	public String getHostname() {
		return hostname;
	}
	
	public void addFileInfo(FileInfo fileInfo) {
		filesInfo.add(fileInfo);
	}
	
	public FileInfo[] getFilesInfo() {
		return (FileInfo[]) filesInfo.toArray(new FileInfo[]{});
	}
	
	public FileInfo getFileInfoWithId(int id) {
		Iterator i = filesInfo.iterator();
		while (i.hasNext()) {
			FileInfo fileInfo = (FileInfo) i.next();
			if (fileInfo.getId() == id)
				return fileInfo;
		}
		
		return null;
	}
}
