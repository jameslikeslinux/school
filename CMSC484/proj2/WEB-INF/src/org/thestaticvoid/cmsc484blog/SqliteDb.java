package org.thestaticvoid.cmsc484blog;

import java.sql.*;
import java.io.*;
import java.util.*;
import java.text.*;

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

	public Article[] getComments(int aid) throws SQLException {
		List comments = new ArrayList();

		Statement stmt = con.createStatement();
		ResultSet rs = stmt.executeQuery("SELECT comments.id,username,datetime,title,content FROM users,comments WHERE users.id=uid AND aid=" + aid);

		while (rs.next())
			comments.add(new Article(
				rs.getInt("id"),
				rs.getString("username"),
				rs.getString("datetime"),
				rs.getString("title"),
				rs.getString("content"),
				new Article[]{}));

		stmt.close();

		return (Article[]) comments.toArray(new Article[]{});
	}

	public Article[] getAllArticles() throws SQLException {
		List articles = new ArrayList();

		Statement stmt = con.createStatement();
		ResultSet rs = stmt.executeQuery("SELECT articles.id,username,datetime,title,content FROM users,articles WHERE users.id=uid");

		while (rs.next())
			articles.add(new Article(
				rs.getInt("id"),
				rs.getString("username"),
				rs.getString("datetime"),
				rs.getString("title"),
				rs.getString("content"),
				getComments(rs.getInt("id"))));

		stmt.close();

		return (Article[]) articles.toArray(new Article[]{});
	}

	// This is a stupid lazy hack
	public Article getArticle(int aid) throws SQLException {
		Article[] articles = getAllArticles();
		for (int i = 0; i < articles.length; i++)
			if (articles[i].getId() == aid)
				return articles[i];

		return null;
	}

	public int getUid(String username) throws SQLException {
		Statement stmt = con.createStatement();
		ResultSet rs = stmt.executeQuery("SELECT id FROM users WHERE username='" + username + "'");
		int uid = -1;
		if (rs.next())
			uid = rs.getInt("id");
		stmt.close();

		return uid;
	}

	public void createUser(String username, String password, String name) throws SQLException {
		Statement stmt = con.createStatement();
		stmt.executeUpdate("INSERT INTO users (username,password,name,invert) VALUES ('" + username + "','" + Utils.md5(password) + "','" + name + "',0)");
		stmt.close();
	}

	public boolean isCorrectPassword(String username, String password) throws SQLException {
		Statement stmt = con.createStatement();
		ResultSet rs = stmt.executeQuery("SELECT * FROM users WHERE username='" + username + "' AND password='" + Utils.md5(password) + "'");
		boolean correct = rs.next();
		stmt.close();

		return correct;
	}

	public void addArticle(int uid, String title, String content) throws SQLException {
		Statement stmt = con.createStatement();
		stmt.executeUpdate("INSERT INTO articles (uid,datetime,title,content) VALUES (" + uid + ",DATETIME('now'),'" + title + "','" + content + "')");
		stmt.close();
	}

	public void addComment(int aid, int uid, String title, String content) throws SQLException {
		Statement stmt = con.createStatement();
		stmt.executeUpdate("INSERT INTO comments (aid,uid,datetime,title,content) VALUES (" + aid + "," + uid + ",DATETIME('now'),'" + title + "','" + content + "')");
		stmt.close();
	}

	public boolean isInverted(int uid) throws SQLException {
		Statement stmt = con.createStatement();
		ResultSet rs = stmt.executeQuery("SELECT invert FROM users WHERE id=" + uid);
		if (!rs.next())
			return false;
		boolean invert = rs.getBoolean("invert");
		stmt.close();

		return invert;
	}

	public void setInverted(int uid, boolean invert) throws SQLException {
		Statement stmt = con.createStatement();
		stmt.executeUpdate("UPDATE users SET invert=" + (invert ? 1 : 0) + " WHERE id=" + uid);
		stmt.close();
	}
}
