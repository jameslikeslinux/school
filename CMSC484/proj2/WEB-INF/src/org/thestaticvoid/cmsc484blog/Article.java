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

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
	}

	public String getUsername() {
		return username;
	}

	public void setUsername(String username) {
		this.username = username;
	}
	
	public String getTimestamp() {
		return timestamp;
	}

	public void setTimestamp(String timestamp) {
		this.timestamp = timestamp;
	}

	public String getTitle() {
		return title;
	}

	public void setTitle(String title) {
		this.title = title;
	}

	public String getContent() {
		return content;
	}

	public void setContent(String content) {
		this.content = content;
	}

	/**
	 * @return	Comments in the form of an array if this is an article, <code>null</code> if this is a comment.
	 */
	public Article[] getComments() {
		return comments;
	}

	public void setComments(Article[] comments) {
		this.comments = comments;
	}
}
