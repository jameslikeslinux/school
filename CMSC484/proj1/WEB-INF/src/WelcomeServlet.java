import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;

public class WelcomeServlet extends HttpServlet {
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		response.setContentType("text/html");
		PrintWriter out = response.getWriter();

		Utils.printHeader(out, "Bookstore");
		out.println("<h1>Bookstore</h1>");
		
		String name = Utils.getCookieValue(request, "name");
		if (!name.equals(""))
			out.println("<h2>Welcome, " + name + "!</h2>");

		out.println("<form action=\"SearchServlet\" method=\"get\">");
		out.println("	Query: <input type=\"text\" name=\"query\" /><br />");
		out.println("	Field: <select name=\"field\">");
		out.println("		<option value=\"title\">Title</option>");
		out.println("		<option value=\"author\">Author</option>");
		out.println("		<option value=\"genre\">Genre</option>");
		out.println("	</select>");
		out.println("	<input type=\"submit\" value=\"Search\" />");
		out.println("</form>");
		Utils.printFooter(out);
	}
}
