import java.io.*;

public class Utils {
	public static void printHeader(PrintWriter out, String title) {
		out.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
		out.println("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\"");
		out.println("	\"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">");
		out.println("<html xmlns=\"http://www.w3.org/1999/xhtml\">");
		out.println("	<head>");
		out.println("		<title>" + title + "</title>");
		out.println("		<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />");
		out.println("		<link href=\"include/master.css\" rel=\"stylesheet\" type=\"text/css\" />");
		out.println("	</head>");
		out.println("	<body>");
	}

	public static void printFooter(PrintWriter out) {
		out.println("	</body>");
		out.println("</html>");
	}
}
