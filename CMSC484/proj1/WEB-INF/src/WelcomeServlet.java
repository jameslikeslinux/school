import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;

public class WelcomeServlet extends HttpServlet {
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		response.setContentType("text/html");
		PrintWriter out = response.getWriter();

		Utils.printHeader(out, "Welcome");

		try {
			out.println("<p>" + SqliteItemDB.getSingleton().searchByKey("B0002").getTitle() + "</p>");
		} catch (Exception e) {
			e.printStackTrace(out);
		}

		Utils.printFooter(out);

		out.close();
	}
}
