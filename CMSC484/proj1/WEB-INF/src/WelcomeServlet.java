import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;

public class WelcomeServlet extends HttpServlet {
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		response.setContentType("text/html");
		PrintWriter out = response.getWriter();

		try {
			out.println("<p>" + SqliteItemDB.getSingleton().searchByKey("B0002").getTitle() + "</p>");
		} catch (Exception e) {
			e.printStackTrace(out);
		}

		out.close();
	}
}
