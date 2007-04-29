package org.thestaticvoid.cmsc484blog;

public class Article {
	private String username, timestamp, title, content;

	public Article(String username, String timestamp, String title, String content) {
		this.username = username;
		this.timestamp = timestamp;
		this.title = title;
		this.content = content;
	}

	public String getUsername() {
		return username;
	}
	
	public String getTimestamp() {
		return timestamp;
	}

	public String getTitle() {
		return title;
	}

	public String getContent() {
		return content;
	}
}
