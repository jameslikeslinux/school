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
		con = DriverManager.getConnection("jdbc:sqlite:" + new File(getClass().getProtectionDomain().getCodeSource().getLocation().getPath()).getParentFile().getParent() + File.separator + "db" + File.separator + "proj2.db");
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
}
