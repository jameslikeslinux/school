package org.thestaticvoid.cmsc484blog;

/**
 * Represents an article or a comment.
 *
 * @author	James Lee
 * @version	20070501
 */
public class Article {
	private int id;
	private String username, timestamp, title, content;
	private Article[] comments;

	public Article(int id, String username, String timestamp, String title, String content, Article[] comments) {
		this.id = id;
		this.username = username;
		this.timestamp = timestamp;
		this.title = title;
		this.content = content;
		this.comments = comments;
	}

	public int getId() {
		return id;
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

	/**
	 * @return	Comments in the form of an array if this is an article, <code>null</code> if this is a comment.
	 */
	public Article[] getComments() {
		return comments;
	}
}
