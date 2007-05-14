package org.thestaticvoid.msync;

/**
 * Parses and creates strings containing information about files.
 * 
 * @author James Lee
 * @version 20070509
 */
public class FileInfo {
	private int id;
	private String name, md5;
	private long size;
	
	public FileInfo(int id, String name, long size, String md5) {
		this.id = id;
		this.name = name;
		this.size = size;
		this.md5 = md5;
	}
	
	public static FileInfo parse(String string) {
		String[] tokens = string.split(" ");
		
		if (tokens.length == 5 && tokens[0].equals("FILE"))
			try {
				return new FileInfo(Integer.parseInt(tokens[1]), tokens[2], Long.parseLong(tokens[3]), tokens[4]);
			} catch (NumberFormatException e) {
				return null;
			}
		
		return null;
	}
	
	public int getId() {
		return id;
	}
	
	public String getName() {
		return name;
	}
	
	public void setName(String name) {
		this.name = name;
	}
	
	public long getSize() {
		return size;
	}
	
	public String getMd5() {
		return md5;
	}
	
	public String toString() {
		return "FILE " + getId() + " " + getName() + " " + getSize() + " " + getMd5();
	}
}
