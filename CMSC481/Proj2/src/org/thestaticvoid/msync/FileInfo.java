package org.thestaticvoid.msync;

public class FileInfo {
	private String name, md5;
	private long size;
	
	public FileInfo(String name, long size, String md5) {
		this.name = name;
		this.size = size;
		this.md5 = md5;
	}
	
	public static FileInfo parse(String string) {
		String[] tokens = string.split(" ");
		
		if (tokens.length == 4 && tokens[0].equals("FILE"))
			try {
				return new FileInfo(tokens[1], Long.parseLong(tokens[2]), tokens[3]);
			} catch (NumberFormatException e) {
				return null;
			}
		
		return null;
	}
	
	public String getName() {
		return name;
	}
	
	public long getSize() {
		return size;
	}
	
	public String getMd5() {
		return md5;
	}
	
	public String toString() {
		return "FILE " + getName() + " " + getSize() + " " + getMd5();
	}
}
