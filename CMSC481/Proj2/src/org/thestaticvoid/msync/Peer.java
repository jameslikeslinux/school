package org.thestaticvoid.msync;

public class Peer {
	private String hostname;
	
	private Peer(String hostname) {
		this.hostname = hostname;
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
}
