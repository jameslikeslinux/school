import java.sql.*;
import java.io.*;

public class SqliteItemDB implements ItemDB {
	private static SqliteItemDB singleton;
	private Connection con;

	private SqliteItemDB() throws ClassNotFoundException, SQLException {
		Class.forName("org.sqlite.JDBC");
		con = DriverManager.getConnection("jdbc:sqlite:" + new File(getClass().getProtectionDomain().getCodeSource().getLocation().getPath()).getParentFile().getParent() + "db" + File.separator + "proj1.db");
	}

	protected void finalize() throws Throwable {
		con.close();
	}

	public static SqliteItemDB getSingleton() throws ClassNotFoundException, SQLException {
		if (singleton == null)
			singleton = new SqliteItemDB();

		return singleton;
	}

	public Item searchByKey(String key) throws SQLException {
		Item item = null;

		Statement stmt = con.createStatement();
		ResultSet rs = stmt.executeQuery("SELECT * FROM books WHERE id like '%" + key + "%'");
		
		if (rs.next())
			item = new Item(
				rs.getString("id"),
				rs.getString("title"),
				rs.getString("author"),
				rs.getString("genre"),
				rs.getString("binding"),
				rs.getString("price"),
				rs.getString("availability"));

		stmt.close();

		return item;
	}

	public Item[] searchTitle(String ss) {
		return null;
	}

	public Item[] searchAuthor(String ss) {
		return null;
	}

	public Item[] searchGenre(String ss) {
		return null;
	}
}
