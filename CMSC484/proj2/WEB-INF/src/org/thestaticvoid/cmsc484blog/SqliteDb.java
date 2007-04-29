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

	public Article[] getAllArticles() throws SQLException {
		List articles = new ArrayList();

		Statement stmt = con.createStatement();
		ResultSet rs = stmt.executeQuery("SELECT username,datetime,title,content FROM users,articles WHERE users.id=uid");

		while (rs.next()) {
			java.sql.Date date = rs.getDate("datetime");
			java.sql.Time time = rs.getTime("datetime");

			articles.add(new Article(
				rs.getString("username"),
				DateFormat.getDateInstance(DateFormat.MEDIUM).format(date) + " at " + DateFormat.getDateInstance(DateFormat.SHORT).format(time),
				rs.getString("title"),
				rs.getString("content")));
		}

		stmt.close();

		return (Article[]) articles.toArray(new Article[]{});
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
		stmt.executeUpdate("INSERT INTO users (username,password,name) VALUES ('" + username + "','" + Utils.md5(password) + "','" + name + "')");
		stmt.close();
	}

	public boolean isCorrectPassword(String username, String password) throws SQLException {
		Statement stmt = con.createStatement();
		ResultSet rs = stmt.executeQuery("SELECT * FROM users WHERE username='" + username + "' AND password='" + Utils.md5(password) + "'");
		boolean correct = rs.next();
		stmt.close();

		return correct;
	}
}
