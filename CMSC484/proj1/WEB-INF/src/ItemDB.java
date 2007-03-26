/** 
 * BT Library ItemDB interface.
 * Any database of items should implement this interface.
 * &copy; 2003 Bob Tarr
 */

public interface ItemDB {
	public Item[] search(String field, String where, boolean exact) throws Exception;
	public Item searchByKey(String key) throws Exception;
	public Item[] searchTitle(String title) throws Exception;
	public Item[] searchAuthor(String author) throws Exception;
	public Item[] searchGenre(String genre) throws Exception;
}
