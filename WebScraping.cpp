#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <string>
#include <regex>

// Callback function to handle data received from cURL
size_t write_data(void* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

// Function to fetch HTML content using cURL
std::string fetch_html(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string html_data;

    curl = curl_easy_init(); // Initialize cURL
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());  // Set URL
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); // Set callback function
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html_data); // Pass string to store data
        res = curl_easy_perform(curl); // Perform the request
        curl_easy_cleanup(curl); // Clean up

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "cURL Error: " << curl_easy_strerror(res) << std::endl;
            return ""; // Return empty string on error
        }
    }
    return html_data; // Return the fetched HTML data
}

// Function to extract product data from HTML
void extract_product_data(const std::string& html) {
    // Regex to match product information
    std::regex product_regex(R"(<div class="product">.*?<h2 class="product-name">(.+?)</h2>.*?<span class="price">\$([\d.]+)</span>.*?<span class="rating">(\d+(\.\d+)?)</span>)", std::regex::icase | std::regex::dotall);
    std::smatch matches;

    // Open CSV file for writing
    std::ofstream output_file("products.csv");
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output file." << std::endl;
        return;
    }
    output_file << "Product Name,Price,Rating\n";  // CSV Header

    std::string::const_iterator search_start(html.cbegin());
    while (std::regex_search(search_start, html.cend(), matches, product_regex)) {
        // Extract data using regex matches
        std::string product_name = matches[1].str();
        std::string price = matches[2].str();
        std::string rating = matches[3].str();

        // Write the extracted data to CSV
        output_file << product_name << "," << price << "," << rating << "\n";

        search_start = matches.suffix().first; // Move to the next match
    }

    output_file.close();
    std::cout << "Data extracted and saved to products.csv" << std::endl;
}

int main() {
    // Specify the URL of the e-commerce page to scrape
    std::string url = "https://example.com/products"; // Replace with the actual URL

    // Fetch the HTML content from the website
    std::string html_content = fetch_html(url);

    if (!html_content.empty()) {
        // Extract product information (names, prices, and ratings)
        extract_product_data(html_content);
    } else {
        std::cerr << "Failed to fetch HTML content." << std::endl;
    }

    return 0;
}
