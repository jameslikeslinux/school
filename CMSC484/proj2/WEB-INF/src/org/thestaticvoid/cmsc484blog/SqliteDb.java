package org.thestaticvoid.cmsc484blog;

import java.sql.*;
import java.io.*;
import java.util.*;
import java.text.*;

/**
 * Provides high level access to the database.
 *
 * @author	James Lee
 * @version	20070501
 */
public class SqliteDb {
	private static SqliteDb singleton;
	private Connection con;

	private SqliteDb() throws ClassNotFoundException, SQLException {
		Class.forName("org.sqlite.JDBC");
		con = DriverManager.getConnection("jdbc:sqlite:" + new File(getClass().getProtectionDomain().getCodeSource().getLocation().getPath()).getParentFile().getParentFile().getParentFile().getParentFile().getParent() + File.separator + "db" + File.separator + "proj2.db");
	}

	protected void finalize() throws Throwable {
		con.close();
	}

	public static SqliteDb getSingleton() throws ClassNotFoundException, SQLException {
		if (singleton == null)
			singleton = new SqliteDb();

		return singleton;
	}

	/**
	 * @param aid	The positive unique key for the article.
	 * @return	All comments for the given article.
	 * @see		Article
	 */
	public Article[] getComments(int aid) throws SQLException {
		List comments = new ArrayList();

		Statement stmt = con.createStatement();
		ResultSet rs = stmt.executeQuery("SELECT comments.id,username,datetime,title,content FROM users,comments WHERE users.id=uid AND aid=" + aid);

		while (rs.next()) {
			Article comment = new Article();
			comment.setId(rs.getInt("id"));
			comment.setUsername(rs.getString("username"));
			comment.setTimestamp(rs.getString("datetime"));
			comment.setTitle(rs.getString("title"));
			comment.setContent(rs.getString("content"));
			comment.setComments(new Article[]{});
			comments.add(comment);
		}
			
		stmt.close();

		return (Article[]) comments.toArray(new Article[]{});
	}

	/**
	 * @return	All articles in the database.
	 * @see		Article
	 */
	public Article[] getAllArticles() throws SQLException {
		List articles = new ArrayList();

		Statement stmt = con.createStatement();
		ResultSet rs = stmt.executeQuery("SELECT articles.id,username,datetime,title,content FROM users,articles WHERE users.id=uid");

		while (rs.next()) {
			Article article = new Article();
			article.setId(rs.getInt("id"));
			article.setUsername(rs.getString("username"));
			article.setTimestamp(rs.getString("datetime"));
			article.setTitle(rs.getString("title"));
			article.setContent(rs.getString("content"));
			article.setComments(getComments(rs.getInt("id")));
			articles.add(article);
		}

		stmt.close();

		return (Article[]) articles.toArray(new Article[]{});
	}

	/**
	 * @param aid	The positive unique key for the article.
	 * @return	The requested article.
	 * @see		Article
	 */
	public Article getArticle(int aid) throws SQLException {
		// This is a stupid lazy hack
		Article[] articles = getAllArticles();
		for (int i = 0; i < articles.length; i++)
			if (articles[i].getId() == aid)
				return articles[i];

		return null;
	}

	/**
	 * @param username	The username for which you want to know the uid.
	 * @return		The positive unique id for the user or -1 if the
	 * 			user does not exist.
	 */
	public int getUid(String username) throws SQLException {
		Statement stmt = con.createStatement();
		ResultSet rs = stmt.executeQuery("SELECT id FROM users WHERE username='" + username + "'");
		int uid = -1;
		if (rs.next())
			uid = rs.getInt("id");
		stmt.close();

		return uid;
	}

	/**
	 * Create a user in the database.  Does not check if user exists.
	 *
	 * @param password	Unhashed password.
	 */
	public void createUser(String username, String password, String name) throws SQLException {
		Statement stmt = con.createStatement();
		stmt.executeUpdate("INSERT INTO users (username,password,name,invert) VALUES ('" + username + "','" + Utils.md5(password) + "','" + name + "',0)");
		stmt.close();
	}

	/**
	 * @param password	Unhashed password.
	 * @return		<code>true</code> if correct u/p combo,\
	 * 			<code>false</code> otherwise.
	 */
	public boolean isCorrectPassword(String username, String password) throws SQLException {
		Statement stmt = con.createStatement();
		ResultSet rs = stmt.executeQuery("SELECT * FROM users WHERE username='" + username + "' AND password='" + Utils.md5(password) + "'");
		boolean correct = rs.next();
		stmt.close();

		return correct;
	}

	/**
	 * Add an article to the database.  Parameters not checked.
	 *
	 * @param uid		The id of the author of the article.
	 * @param title		The title of the article.
	 * @param content	The content of the article.
	 */
	public void addArticle(int uid, String title, String content) throws SQLException {
		Statement stmt = con.createStatement();
		stmt.executeUpdate("INSERT INTO articles (uid,datetime,title,content) VALUES (" + uid + ",DATETIME('now'),'" + title + "','" + content + "')");
		stmt.close();
	}

	/**
	 * Add a comment to an article.  Parameters not checked.
	 *
	 * @param aid		The id of the article on which to comment.
	 * @param uid		The id of the author making the comment.
	 * @param title		The title of the comment.
	 * @param content	The content of the comment.
	 */
	public void addComment(int aid, int uid, String title, String content) throws SQLException {
		Statement stmt = con.createStatement();
		stmt.executeUpdate("INSERT INTO comments (aid,uid,datetime,title,content) VALUES (" + aid + "," + uid + ",DATETIME('now'),'" + title + "','" + content + "')");
		stmt.close();
	}
	
	/**
	 * Gets the user's style preference.  uid validity not checked.
	 */
	public boolean isInverted(int uid) throws SQLException {
		Statement stmt = con.createStatement();
		ResultSet rs = stmt.executeQuery("SELECT invert FROM users WHERE id=" + uid);
		if (!rs.next())
			return false;
		boolean invert = rs.getBoolean("invert");
		stmt.close();

		return invert;
	}

	/**
	 * Sets the user's style preference.  uid validity not checked.
	 */
	public void setInverted(int uid, boolean invert) throws SQLException {
		Statement stmt = con.createStatement();
		stmt.executeUpdate("UPDATE users SET invert=" + (invert ? 1 : 0) + " WHERE id=" + uid);
		stmt.close();
	}
}
