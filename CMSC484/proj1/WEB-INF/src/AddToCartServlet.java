import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;
import java.util.*;

public class AddToCartServlet extends HttpServlet {
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
			Utils.printFooter(out);
			return;
		}

		HttpSession session = request.getSession();
		List cart = (List) session.getAttribute("cart");

		if (cart == null) {
			cart = new ArrayList();
			cart.add(item);
			session.setAttribute("cart", cart);
		} else if (!cart.contains(item))
			cart.add(item);

		Utils.printHeader(out, "Added to Cart");
		out.println("<h1>Added to Cart</h1>");
		for (int i = 0; i < cart.size(); i++) {
			item = (Item) cart.get(i);
			out.println("<a href=\"DisplayItemServlet?id=" + item.getId() + "\">" + item.getTitle() + "</a><br />");
		}
		Utils.printFooter(out);
	}
}
