/** 
 * BT Library ItemDB interface.
 * Any database of items should implement this interface.
 * &copy; 2003 Bob Tarr
 */

public interface ItemDB {
	public Item searchByKey(String key) throws Exception;
	public Item[] searchTitle(String ss) throws Exception;
	public Item[] searchAuthor(String ss) throws Exception;
	public Item[] searchGenre(String ss) throws Exception;
}
