import java.sql.*;
import java.io.*;
import java.util.*;

public class SqliteItemDB implements ItemDB {
	private static SqliteItemDB singleton;
	private Connection con;

	private SqliteItemDB() throws ClassNotFoundException, SQLException {
		Class.forName("org.sqlite.JDBC");
		con = DriverManager.getConnection("jdbc:sqlite:" + new File(getClass().getProtectionDomain().getCodeSource().getLocation().getPath()).getParentFile().getParent() + File.separator + "db" + File.separator + "proj1.db");
	}

	protected void finalize() throws Throwable {
		con.close();
	}

	public static SqliteItemDB getSingleton() throws ClassNotFoundException, SQLException {
		if (singleton == null)
			singleton = new SqliteItemDB();

		return singleton;
	}

	private Item[] search(String column, String like) throws SQLException {
		List items = new ArrayList();

		Statement stmt = con.createStatement();
		ResultSet rs = stmt.executeQuery("SELECT * FROM books WHERE " + column + " like '%" + like + "%'");

		while (rs.next())
			items.add(new Item(
				rs.getString("id"),
				rs.getString("title"),
				rs.getString("author"),
				rs.getString("genre"),
				rs.getString("binding"),
				rs.getString("price"),
				rs.getString("availability")));

		stmt.close();

		return (Item[]) items.toArray(new Item[]{});
	}

	public Item searchByKey(String key) throws SQLException {
		Item[] items = search("id", key);
		return (items.length > 0) ? items[0] : null;
	}

	public Item[] searchTitle(String title) throws SQLException {
		return search("title", title);
	}

	public Item[] searchAuthor(String author) throws SQLException {
		return search("author", author);
	}

	public Item[] searchGenre(String genre) throws SQLException {
		return search("genre", genre);
	}
}
