import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;

public class DisplayItemServlet extends HttpServlet {
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		response.setContentType("text/html");
		PrintWriter out = response.getWriter();

		Item item = null;
		String id = request.getParameter("id");
		if (!(id == null || id.equals("")))
			try {
				item = SqliteItemDB.getSingleton().searchByKey(id);
			} catch (Exception e) {
				e.printStackTrace();
			}

		if (item == null) {
			Utils.printHeader(out, "Unknown Item");
			out.println("<h1>Unknown Item</h1>");
		} else {
			Utils.printHeader(out, item.getAuthor() + " - " + item.getTitle());
			out.println("<h1>" + item.getAuthor() +  " - " + item.getTitle() + "</h1>");
			out.println("<ul>");
			out.println("	<li>Genre: " + item.getGenre() + "</li>");
			out.println("	<li>Binding: " + item.getBinding() + "</li>");
			out.println("	<li>Price: " + item.getPrice() + "</li>");
			out.println("	<li>Availability: " + item.getAvailability() + "</li>");
			out.println("</ul>");
		}

		Utils.printFooter(out);

		out.close();
	}
}
