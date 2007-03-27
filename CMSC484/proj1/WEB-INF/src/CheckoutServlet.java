import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;
import java.util.*;
import java.text.*;

public class CheckoutServlet extends HttpServlet {
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		response.setContentType("text/html");
		PrintWriter out = response.getWriter();

		HttpSession session = request.getSession();
		List cart = (List) session.getAttribute("cart");

		Utils.printHeader(out, "Checkout");
		out.println("<h1>Checkout</h1>");

		if (cart == null || cart.size() == 0)
			out.println("<p>Your cart is empty.</p>");
		else {
			double total = 0;

			for (int i = 0; i < cart.size(); i++) {
				Item item = (Item) cart.get(i);
				total += item.getPrice();
				out.println("<a href=\"DisplayItemServlet?id=" + item.getId() + "\">" + item.getTitle() + "</a> - " + new DecimalFormat("$#0.00").format(item.getPrice()) + "<br />");
			}

			out.println("<div style=\"font-weight: bold\">Total: " + new DecimalFormat("$#.00").format(total) + "</div><br />");
			out.println("<form action=\"ConfirmServlet\" method=\"get\">");
			out.println("	Name: <input type=\"text\" name=\"name\" value=\"" + Utils.getCookieValue(request, "name") + "\" />");
			out.println("	<input type=\"submit\" value=\"Confirm Checkout\" />");
			out.println("</form>");
		}

		Utils.printFooter(out);
	}
}
