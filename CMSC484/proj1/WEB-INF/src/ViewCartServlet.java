import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;
import java.util.*;
import java.text.*;

public class ViewCartServlet extends HttpServlet {
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		response.setContentType("text/html");
		PrintWriter out = response.getWriter();

		HttpSession session = request.getSession();
		List cart = (List) session.getAttribute("cart");

		Utils.printHeader(out, "Cart Contents");
		out.println("<h1>Cart Contents</h1>");

		if (cart == null || cart.size() == 0)
			out.println("<p>Your cart is empty.</p>");
		else {
			for (int i = 0; i < cart.size(); i++) {
				Item item = (Item) cart.get(i);
				out.println("<a href=\"DisplayItemServlet?id=" + item.getId() + "\">" + item.getTitle() + "</a> - " + new DecimalFormat("$#0.00").format(item.getPrice()) + "<br />");
			}

			out.println("<br /><a href=\"CheckoutServlet\">Proceed to Checkout</a>");
		}

		Utils.printFooter(out);
	}
}
