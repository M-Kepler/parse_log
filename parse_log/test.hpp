/*
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
static void set_value(xmlXPathContextPtr context, const xmlChar *xpath, const xmlChar *value) {
	xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath, context);
	if (result) {
		xmlNodeSetPtr nodeset = result->nodesetval;
		xmlNodeSetContent(nodeset->nodeTab[0], value);
		xmlXPathFreeObject(result);
	}
}
static void get_request(char *buffer, int *len, const char *stock_code) {
	xmlChar *buff;
	xmlDocPtr doc = xmlParseFile("stock.xml");
	if (doc == NULL) {
		printf("xmlParseFile failed/n");
		return;
	}
	xmlXPathContextPtr context = xmlXPathNewContext(doc);
	if (context == NULL) {
		printf("xmlXPathNewContext failed/n");
		return;
	}
	xmlXPathRegisterNs(context, (const xmlChar *)"soapenv", (const xmlChar *)"http://schemas.xmlsoap.org/soap/envelope/");
	xmlXPathRegisterNs(context, (const xmlChar *)"web", (const xmlChar *)"http://WebXml.com.cn/");
	set_value(context, (const xmlChar *)"//web:theStockCode", (const xmlChar *)stock_code);
	xmlDocDumpMemory(doc, &buff, len);
	strcpy(buffer, (char *)buff);
	printf("%s/n", buffer);
	xmlFree(buff);
	xmlXPathFreeContext(context);
	xmlFreeDoc(doc);
	xmlCleanupParser();
}
int main(int argc, char **argv) {
	const char *stock_code = argv[1];
	char buffer[2048];
	int len = 0;
	get_request(buffer, &len, stock_code);
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: text/xml;charset=UTF-8");
	headers = curl_slist_append(headers, "SOAPAction: /"http://WebXml.com.cn/getStockInfoByCode/"");
	CURL * curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://webservice.webxml.com.cn/WebServices/ChinaStockWebService.asmx");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(buffer));
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_perform(curl);
		// always cleanup
		curl_easy_cleanup(curl);
		curl_slist_free_all(headers);
	}
	return 0;
}
*/
