import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;

public class WelcomeServlet extends HttpServlet {
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		response.setContentType("text/html");
		PrintWriter out = response.getWriter();

		out.println("<p>" + new File(getClass().getProtectionDomain().getCodeSource().getLocation().getPath()).getParent() + "</p>");

		out.close();
	}
}
