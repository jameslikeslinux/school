package org.thestaticvoid.cmsc484blog;

/**
 * Used by servlets to create a dynamic HTML header
 *
 * @author	James Lee
 * @version	20070501
 */
public class PageTitle {
	private String pageTitle;
	private String css;

	public String getPageTitle() {
		return pageTitle;
	}

	public void setPageTitle(String pageTitle) {
		this.pageTitle = pageTitle;
	}

	public String getCss() {
		return css;
	}

	public void setCss(String css) {
		this.css = css;
	}
}
