import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;
import java.util.*;
import java.text.*;

public class ConfirmServlet extends HttpServlet {
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		response.setContentType("text/html");
		PrintWriter out = response.getWriter();

		String name = request.getParameter("name");
		if (name == null || name.equals("")) {
			Utils.printHeader(out, "Invalid Name");
			out.println("<h2>Please provide your name</h2>");
			Utils.printFooter(out);
			return;
		}

		response.addCookie(new Cookie("name", name));

		HttpSession session = request.getSession();
		List cart = (List) session.getAttribute("cart");

		Utils.printHeader(out, "Confirm");
		out.println("<h1>Confirm</h1>");

		if (cart == null || cart.size() == 0)
			out.println("<p>Your cart is empty.</p>");
		else {
			double total = 0;

			for (int i = 0; i < cart.size(); i++) {
				Item item = (Item) cart.get(i);
				total += item.getPrice();
				out.println("<a href=\"DisplayItemServlet?id=" + item.getId() + "\">" + item.getTitle() + "</a> - " + new DecimalFormat("$#0.00").format(item.getPrice()) + "<br />");
			}

			out.println("<div style=\"font-weight: bold\">Total: " + new DecimalFormat("$#.00").format(total) + "</div>");
			out.println("<p>Name: " + name + "</p>");
			out.println("<form action=\"WelcomeServlet\" method=\"get\">");
			out.println("	<input type=\"submit\" value=\"Checkout\" />");
			out.println("	<a href=\"CheckoutServlet\">Go back</a>");
			out.println("</form>");
		}

		Utils.printFooter(out);
	}
}
