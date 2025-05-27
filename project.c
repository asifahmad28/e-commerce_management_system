#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define constants
#define MAX_USERS 100
#define MAX_PRODUCTS 100
#define MAX_ORDERS 100
#define FILENAME_USERS "users.txt"
#define FILENAME_PRODUCTS "products.txt"
#define FILENAME_ORDERS "orders.txt"
#define FILENAME_ORDER_HISTORY "order_history.txt"
#define PASSWORD_LENGTH 50

// User structure
typedef struct {
    char username[50];
    char password[PASSWORD_LENGTH];
    int isAdmin; // 1 for admin, 0 for regular user
} User;

// Product structure
typedef struct {
    char name[50];
    char category[50];
    float price;
    int stock;
    float discount; // Discount percentage
    float rating;
    char reviews[100];
} Product;

// Order structure
typedef struct {
    int orderId;
    char username[50];
    char productName[50];
    int quantity;
    float totalPrice;
    char address[100];
    char paymentMethod[20];
} Order;

// Global arrays to store users, products, and orders
User users[MAX_USERS];
Product products[MAX_PRODUCTS];
Order orders[MAX_ORDERS];
int userCount = 0;
int productCount = 0;
int orderCount = 0;
int lastOrderId = 0;
int lastSavedOrderId = 0;  // Track the last order ID assigned

// Function prototypes
void loadUsers();
void saveUsers();
void loadProducts();
void saveProducts();
void loadOrders();
void saveOrders();
void saveOrderHistory();
void registerUser();
int loginUser(char *username);
void adminPanel();
void userPanel(char *username);
void addProduct();
void removeProduct();
void updateDiscount();
void viewOrderHistory();
void displayProducts();
void searchProducts();
void addToCart(char *username);
void checkout(char *username);
void updateStock(char *productName, int quantity);
void provideRatingAndReview(char *username);
void clearCart(char *username);
int validateMobileNumber(char *number);
int getIntegerInput(const char *prompt, int min, int max);
float getFloatInput(const char *prompt, float min, float max);
void displayUserOrders(char *username);

// Main function
int main() {
    loadUsers();
    loadProducts();
    loadOrders();

    int choice;
    do {
        printf("\nE-Commerce Management System\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        choice = getIntegerInput("", 1, 3);

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2: {
                char username[50];
                int isAdmin = loginUser(username);
                if (isAdmin == 1) {
                    adminPanel();
                } else if (isAdmin == 0) {
                    userPanel(username);
                }
                break;
            }
            case 3:
                printf("Exiting...\n");
                break;
        }
    } while (choice != 3);

    return 0;
}

// Helper function for safe integer input
int getIntegerInput(const char *prompt, int min, int max) {
    int value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &value) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        if (value >= min && value <= max) {
            return value;
        }
        printf("Please enter a number between %d and %d.\n", min, max);
    }
}

// Helper function for safe float input
float getFloatInput(const char *prompt, float min, float max) {
    float value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%f", &value) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        if (value >= min && value <= max) {
            return value;
        }
        printf("Please enter a number between %.2f and %.2f.\n", min, max);
    }
}

// Load users from file
void loadUsers() {
    FILE *file = fopen(FILENAME_USERS, "r");
    if (file == NULL) {
        printf("No user data found. Starting with an empty list.\n");
        return;
    }
    while (fscanf(file, "%49s %49s %d", users[userCount].username, users[userCount].password, &users[userCount].isAdmin) == 3) {
        userCount++;
        if (userCount >= MAX_USERS) break;
    }
    fclose(file);
}

// Save users to file
void saveUsers() {
    FILE *file = fopen(FILENAME_USERS, "w");
    if (file == NULL) {
        printf("Error saving user data.\n");
        return;
    }
    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s %s %d\n", users[i].username, users[i].password, users[i].isAdmin);
    }
    fclose(file);
}

// Load products from file
void loadProducts() {
    FILE *file = fopen(FILENAME_PRODUCTS, "r");
    if (file == NULL) {
        printf("No product data found. Starting with an empty list.\n");
        return;
    }
    while (fscanf(file, "%49s %49s %f %d %f %f %99[^\n]",
           products[productCount].name,
           products[productCount].category,
           &products[productCount].price,
           &products[productCount].stock,
           &products[productCount].discount,
           &products[productCount].rating,
           products[productCount].reviews) == 7) {
        productCount++;
        if (productCount >= MAX_PRODUCTS) break;
    }
    fclose(file);
}

// Save products to file
void saveProducts() {
    FILE *file = fopen(FILENAME_PRODUCTS, "w");
    if (file == NULL) {
        printf("Error saving product data.\n");
        return;
    }
    for (int i = 0; i < productCount; i++) {
        fprintf(file, "%s %s %.2f %d %.2f %.2f %s\n",
                products[i].name,
                products[i].category,
                products[i].price,
                products[i].stock,
                products[i].discount,
                products[i].rating,
                products[i].reviews);
    }
    fclose(file);
}

// Load orders from file
void loadOrders() {
    FILE *file = fopen(FILENAME_ORDERS, "r");
    if (file == NULL) {
        printf("No order data found. Starting with an empty list.\n");
        return;
    }
    while (fscanf(file, "%d %49s %49s %d %f %19s %99[^\n]",
           &orders[orderCount].orderId,
           orders[orderCount].username,
           orders[orderCount].productName,
           &orders[orderCount].quantity,
           &orders[orderCount].totalPrice,
           orders[orderCount].paymentMethod,
           orders[orderCount].address) == 7) {
        if (orders[orderCount].orderId > lastOrderId) {
            lastOrderId = orders[orderCount].orderId;
        }
        orderCount++;
        if (orderCount >= MAX_ORDERS) break;
    }
    fclose(file);

    // Also load the last saved order ID from history file
    FILE *historyFile = fopen(FILENAME_ORDER_HISTORY, "r");
    if (historyFile != NULL) {
        char line[500];
        int maxId = 0;
        while (fgets(line, sizeof(line), historyFile)) {
            int id;
            if (sscanf(line, "Order ID: %d", &id) == 1) {
                if (id > maxId) {
                    maxId = id;
                }
            }
        }
        lastSavedOrderId = maxId;
        fclose(historyFile);
    }
}

// Save orders to file
void saveOrders() {
    FILE *file = fopen(FILENAME_ORDERS, "w");
    if (file == NULL) {
        printf("Error saving order data.\n");
        return;
    }
    for (int i = 0; i < orderCount; i++) {
        fprintf(file, "%d %s %s %d %.2f %s %s\n",
                orders[i].orderId,
                orders[i].username,
                orders[i].productName,
                orders[i].quantity,
                orders[i].totalPrice,
                orders[i].paymentMethod,
                orders[i].address);
    }
    fclose(file);
}

// Save order history to file
void saveOrderHistory() {
    FILE *file = fopen(FILENAME_ORDER_HISTORY, "a");
    if (file == NULL) {
        printf("Error saving order history.\n");
        return;
    }

    for (int i = 0; i < orderCount; i++) {
        // Only save orders that haven't been saved to history yet
        if (orders[i].orderId > lastSavedOrderId) {
            fprintf(file, "Order ID: %d, User: %s, Product: %s, Qty: %d, Total: %.2f, Method: %s, Address: %s\n",
                    orders[i].orderId,
                    orders[i].username,
                    orders[i].productName,
                    orders[i].quantity,
                    orders[i].totalPrice,
                    orders[i].paymentMethod,
                    orders[i].address);

            // Update the last saved order ID
            if (orders[i].orderId > lastSavedOrderId) {
                lastSavedOrderId = orders[i].orderId;
            }
        }
    }

    fclose(file);
}
// Register a new user
void registerUser() {
    if (userCount >= MAX_USERS) {
        printf("User limit reached. Cannot register more users.\n");
        return;
    }

    User newUser;
    printf("Enter username (max 49 chars): ");
    scanf("%49s", newUser.username);

    // Check if username already exists
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, newUser.username) == 0) {
            printf("Username already exists.\n");
            return;
        }
    }

    printf("Enter password (max %d chars): ", PASSWORD_LENGTH-1);
    scanf("%49s", newUser.password);
    newUser.isAdmin = 0;

    users[userCount++] = newUser;
    saveUsers();
    printf("User registered successfully!\n");
}

// Login a user
int loginUser(char *username) {
    char password[PASSWORD_LENGTH];
    printf("Enter username: ");
    scanf("%49s", username);
    printf("Enter password: ");
    scanf("%49s", password);

    // Check for fixed admin credentials
    if (strcmp(username, "admin") == 0 && strcmp(password, "MATRF") == 0) {
        printf("Admin login successful!\n");
        return 1;
    }

    // Check for regular users
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 &&
            strcmp(users[i].password, password) == 0) {
            printf("Login successful!\n");
            return 0;
        }
    }

    printf("Invalid username or password.\n");
    return -1;
}

// Admin panel
void adminPanel() {
    int choice;
    do {
        printf("\nAdmin Panel\n");
        printf("1. Add Product\n");
        printf("2. Remove Product\n");
        printf("3. Update Discount\n");
        printf("4. View Order History\n");
        printf("5. View Products\n");
        printf("6. Logout\n");
        printf("Enter your choice: ");
        choice = getIntegerInput("", 1, 6);

        switch (choice) {
            case 1:
                addProduct();
                break;
            case 2:
                removeProduct();
                break;
            case 3:
                updateDiscount();
                break;
            case 4:
                viewOrderHistory();
                break;
            case 5:
                displayProducts();
                break;
            case 6:
                printf("Logged out.\n");
                break;
        }
    } while (choice != 6);
}

// User panel
void userPanel(char *username) {
    int choice;
    do {
        printf("\nUser Panel - Welcome %s\n", username);
        printf("1. View Products\n");
        printf("2. Search Products\n");
        printf("3. Add to Cart\n");
        printf("4. View My Orders\n");
        printf("5. Checkout\n");
        printf("6. Review Products\n");
        printf("7. Logout\n");
        printf("Enter your choice: ");
        choice = getIntegerInput("", 1, 7);

        switch (choice) {
            case 1:
                displayProducts();
                break;
            case 2:
                searchProducts();
                break;
            case 3:
                addToCart(username);
                break;
            case 4:
                displayUserOrders(username);
                break;
            case 5:
                checkout(username);
                break;
            case 6:
                provideRatingAndReview(username);
                break;
            case 7:
                printf("Logged out.\n");
                break;
        }
    } while (choice != 7);
}

// Display orders for a specific user
void displayUserOrders(char *username) {
    printf("\nYour Orders:\n");
    int found = 0;
    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].username, username) == 0) {
            printf("Order ID: %d\n", orders[i].orderId);
            printf("Product: %s\n", orders[i].productName);
            printf("Quantity: %d\n", orders[i].quantity);
            printf("Total Price: %.2f\n", orders[i].totalPrice);
            printf("Payment Method: %s\n", orders[i].paymentMethod);
            printf("Delivery Address: %s\n", orders[i].address);
            printf("------------------------\n");
            found = 1;
        }
    }
    if (!found) {
        printf("You have no orders yet.\n");
    }
}

// Add a new product (admin only)
void addProduct() {
    if (productCount >= MAX_PRODUCTS) {
        printf("Product limit reached. Cannot add more products.\n");
        return;
    }

    Product newProduct;
    printf("Enter product name (max 49 chars): ");
    scanf("%49s", newProduct.name);
    printf("Enter product category (max 49 chars): ");
    scanf("%49s", newProduct.category);
    newProduct.price = getFloatInput("Enter product price: ", 0.01, 1000000.0);
    newProduct.stock = getIntegerInput("Enter product stock: ", 1, 1000000);
    newProduct.discount = getFloatInput("Enter product discount (%): ", 0.0, 100.0);
    newProduct.rating = 0;
    strcpy(newProduct.reviews, "No reviews yet.");

    products[productCount++] = newProduct;
    saveProducts();
    printf("Product added successfully!\n");
}

// Remove a product (admin only)
void removeProduct() {
    if (productCount == 0) {
        printf("No products available to delete.\n");
        return;
    }

    displayProducts();
    int serial = getIntegerInput("Enter the serial number of the product to delete (0 to cancel): ", 0, productCount);

    if (serial == 0) {
        printf("Product deletion cancelled.\n");
        return;
    }

    // Shift products after the deleted one
    for (int i = serial - 1; i < productCount - 1; i++) {
        products[i] = products[i + 1];
    }
    productCount--;
    saveProducts();
    printf("Product deleted successfully.\n");
}

// Update discount for a product (admin only)
void updateDiscount() {
    if (productCount == 0) {
        printf("No products available to update.\n");
        return;
    }

    displayProducts();
    int serial = getIntegerInput("Enter the serial number of the product to update discount: ", 1, productCount);
    float discount = getFloatInput("Enter new discount (%): ", 0.0, 100.0);

    products[serial - 1].discount = discount;
    saveProducts();
    printf("Discount updated successfully!\n");
}

// View order history (admin only)
void viewOrderHistory() {
    printf("\nOrder History:\n");
    FILE *file = fopen(FILENAME_ORDER_HISTORY, "r");
    if (file == NULL) {
        printf("No order history found.\n");
        return;
    }

    char line[500];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
}

// Display all products
void displayProducts() {
    if (productCount == 0) {
        printf("No products available.\n");
        return;
    }

    printf("\nProduct List:\n");
    for (int i = 0; i < productCount; i++) {
        printf("Serial: %d\n", i + 1);
        printf("Name: %s\n", products[i].name);
        printf("Category: %s\n", products[i].category);
        printf("Price: %.2f\n", products[i].price);
        printf("Discount: %.2f%%\n", products[i].discount);
        printf("Stock: %d\n", products[i].stock);
        printf("Rating: %.2f\n", products[i].rating);
        printf("Reviews: %s\n", products[i].reviews);
        printf("------------------------\n");
    }
}

// Search products by category or price range
void searchProducts() {
    int choice = getIntegerInput("Search by:\n1. Category\n2. Price Range\n3. Both\nEnter your choice: ", 1, 3);

    if (choice == 1) {
        char category[50];
        printf("Enter category to search: ");
        scanf("%49s", category);
        printf("\nProducts in category '%s':\n", category);

        int found = 0;
        for (int i = 0; i < productCount; i++) {
            if (strcmp(products[i].category, category) == 0) {
                printf("Serial: %d\n", i + 1);
                printf("Name: %s\n", products[i].name);
                printf("Price: %.2f\n", products[i].price);
                printf("Discount: %.2f%%\n", products[i].discount);
                printf("Stock: %d\n", products[i].stock);
                printf("Rating: %.2f\n", products[i].rating);
                printf("Reviews: %s\n", products[i].reviews);
                printf("------------------------\n");
                found = 1;
            }
        }
        if (!found) printf("No products found in this category.\n");

    } else if (choice == 2) {
        float minPrice = getFloatInput("Enter minimum price: ", 0.0, 1000000.0);
        float maxPrice = getFloatInput("Enter maximum price: ", minPrice, 1000000.0);
        printf("\nProducts between %.2f and %.2f:\n", minPrice, maxPrice);

        int found = 0;
        for (int i = 0; i < productCount; i++) {
            if (products[i].price >= minPrice && products[i].price <= maxPrice) {
                printf("Serial: %d\n", i + 1);
                printf("Name: %s\n", products[i].name);
                printf("Category: %s\n", products[i].category);
                printf("Price: %.2f\n", products[i].price);
                printf("Discount: %.2f%%\n", products[i].discount);
                printf("Stock: %d\n", products[i].stock);
                printf("Rating: %.2f\n", products[i].rating);
                printf("Reviews: %s\n", products[i].reviews);
                printf("------------------------\n");
                found = 1;
            }
        }
        if (!found) printf("No products found in this price range.\n");

    } else if (choice == 3) {
        char category[50];
        printf("Enter category to search: ");
        scanf("%49s", category);
        float minPrice = getFloatInput("Enter minimum price: ", 0.0, 1000000.0);
        float maxPrice = getFloatInput("Enter maximum price: ", minPrice, 1000000.0);
        printf("\nProducts in category '%s' and between %.2f and %.2f:\n", category, minPrice, maxPrice);

        int found = 0;
        for (int i = 0; i < productCount; i++) {
            if (strcmp(products[i].category, category) == 0 &&
                products[i].price >= minPrice && products[i].price <= maxPrice) {
                printf("Serial: %d\n", i + 1);
                printf("Name: %s\n", products[i].name);
                printf("Price: %.2f\n", products[i].price);
                printf("Discount: %.2f%%\n", products[i].discount);
                printf("Stock: %d\n", products[i].stock);
                printf("Rating: %.2f\n", products[i].rating);
                printf("Reviews: %s\n", products[i].reviews);
                printf("------------------------\n");
                found = 1;
            }
        }
        if (!found) printf("No products found matching these criteria.\n");
    }
}

// Add product to cart
void addToCart(char *username) {
    displayProducts();
    if (productCount == 0) return;

    int serial = getIntegerInput("Enter the serial number of the product to add to cart (0 to cancel): ", 0, productCount);
    if (serial == 0) return;

    int quantity = getIntegerInput("Enter quantity: ", 1, products[serial-1].stock);

    if (products[serial - 1].stock >= quantity) {
        Order newOrder;
        newOrder.orderId = ++lastOrderId; // Assign a new order ID
        strncpy(newOrder.username, username, 49);
        strncpy(newOrder.productName, products[serial - 1].name, 49);
        newOrder.quantity = quantity;
        newOrder.totalPrice = products[serial - 1].price * quantity * (1 - products[serial - 1].discount / 100);

        printf("Enter your address: ");
        getchar(); // Clear buffer
        fgets(newOrder.address, 100, stdin);
        newOrder.address[strcspn(newOrder.address, "\n")] = 0;

        strcpy(newOrder.paymentMethod, "Pending");

        if (orderCount < MAX_ORDERS) {
            orders[orderCount++] = newOrder;
            saveOrders();
            printf("Product added to cart successfully! Order ID: %d\n", newOrder.orderId);
        } else {
            printf("Cart is full. Cannot add more items.\n");
        }
    } else {
        printf("Insufficient stock.\n");
    }
}

// Checkout and place order
void checkout(char *username) {
    float total = 0;
    printf("\nYour Cart:\n");

    int hasItems = 0;
    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].username, username) == 0 && strcmp(orders[i].paymentMethod, "Pending") == 0) {
            printf("Order ID: %d\n", orders[i].orderId);
            printf("Product: %s, Quantity: %d, Total Price: %.2f\n",
                   orders[i].productName,
                   orders[i].quantity,
                   orders[i].totalPrice);
            total += orders[i].totalPrice;
            hasItems = 1;
        }
    }

    if (!hasItems) {
        printf("Your cart is empty. No payment required.\n");
        getchar(); // Wait for user input
        return;
    }

    printf("Total Amount: %.2f\n", total);

    // Payment method selection
    printf("Choose payment method:\n");
    printf("1. Visa/Mastercard\n");
    printf("2. Mobile Banking (Bkash/Nagad)\n");
    printf("3. Cash on Delivery\n");
    printf("4. Back to Main Menu\n");
    int paymentChoice = getIntegerInput("Enter your choice: ", 1, 4);

    if (paymentChoice == 4) {
        printf("Returning to the main menu...\n");
        return;
    }

    // Process payment
    switch (paymentChoice) {
        case 1: {
            char accountNumber[20], pin[10];
            printf("Enter your account number: ");
            scanf("%19s", accountNumber);
            printf("Enter your PIN: ");
            scanf("%9s", pin);
            printf("Processing payment...\n");
            printf("Payment Succeed!\n");
            printf("Your %.2f Taka Paid\n", total);

            // Update payment method for all pending orders
            for (int i = 0; i < orderCount; i++) {
                if (strcmp(orders[i].username, username) == 0 && strcmp(orders[i].paymentMethod, "Pending") == 0) {
                    strncpy(orders[i].paymentMethod, "Visa/Mastercard", 19);
                    updateStock(orders[i].productName, orders[i].quantity);
                }
            }
            break;
        }
        case 2: {
            int mobileChoice = getIntegerInput("Choose:\n1. Bkash\n2. Nagad\nEnter choice: ", 1, 2);
            char mobileNumber[15], pin[10];
            do {
                printf("Enter your mobile number (11 digits): ");
                scanf("%14s", mobileNumber);
            } while (!validateMobileNumber(mobileNumber));
            printf("Enter your PIN: ");
            scanf("%9s", pin);
            printf("Processing payment...\n");
            printf("Payment Succeed!\n");
            printf("Your %.2f Taka Paid\n", total);

            // Update payment method for all pending orders
            for (int i = 0; i < orderCount; i++) {
                if (strcmp(orders[i].username, username) == 0 && strcmp(orders[i].paymentMethod, "Pending") == 0) {
                    strncpy(orders[i].paymentMethod, (mobileChoice == 1) ? "Bkash" : "Nagad", 19);
                    updateStock(orders[i].productName, orders[i].quantity);
                }
            }
            break;
        }
        case 3: {
            printf("You have chosen Cash on Delivery. Payment will be made upon delivery.\n");

            // Update payment method for all pending orders
            for (int i = 0; i < orderCount; i++) {
                if (strcmp(orders[i].username, username) == 0 && strcmp(orders[i].paymentMethod, "Pending") == 0) {
                    strncpy(orders[i].paymentMethod, "Cash on Delivery", 19);
                    updateStock(orders[i].productName, orders[i].quantity);
                }
            }
            break;
        }
    }

    // Save to order history and clear pending orders
    saveOrderHistory();
    saveOrders();
    printf("Thank you for your purchase!\n");
}

// Update stock after purchase
void updateStock(char *productName, int quantity) {
    for (int i = 0; i < productCount; i++) {
        if (strcmp(products[i].name, productName) == 0) {
            products[i].stock -= quantity;
            if (products[i].stock <= 0) {
                // Auto delete out-of-stock products
                for (int j = i; j < productCount - 1; j++) {
                    products[j] = products[j + 1];
                }
                productCount--;
            }
            saveProducts();
            return;
        }
    }
}

// Provide rating and review after checkout
void provideRatingAndReview(char *username) {
    displayUserOrders(username);
    if (orderCount == 0) return;

    int orderId = getIntegerInput("Enter the Order ID you want to review (0 to cancel): ", 0, lastOrderId);
    if (orderId == 0) return;

    // Find the product in the order
    char productToReview[50] = "";
    for (int i = 0; i < orderCount; i++) {
        if (orders[i].orderId == orderId && strcmp(orders[i].username, username) == 0) {
            strcpy(productToReview, orders[i].productName);
            break;
        }
    }

    if (strlen(productToReview) == 0) {
        printf("Order ID not found or doesn't belong to you.\n");
        return;
    }

    // Find the product in products
    for (int i = 0; i < productCount; i++) {
        if (strcmp(products[i].name, productToReview) == 0) {
            float rating = getFloatInput("Enter your rating (0-5): ", 0.0, 5.0);
            printf("Enter your review: ");
            getchar(); // Clear buffer
            fgets(products[i].reviews, 100, stdin);
            products[i].reviews[strcspn(products[i].reviews, "\n")] = 0;
            products[i].rating = rating;

            saveProducts();
            printf("Thank you for your feedback!\n");
            return;
        }
    }

    printf("Product not found.\n");
}

// Validate mobile number (11 digits, starting with 018/019/017/013/014/015/016)
int validateMobileNumber(char *number) {
    if (strlen(number) != 11) {
        printf("Mobile number must be 11 digits.\n");
        return 0;
    }

    for (int i = 0; i < 11; i++) {
        if (!isdigit(number[i])) {
            printf("Mobile number must contain only digits.\n");
            return 0;
        }
    }

    char prefix[4];
    strncpy(prefix, number, 3);
    prefix[3] = '\0';
    const char *validPrefixes[] = {"018", "019", "017", "013", "014", "015", "016"};

    for (int i = 0; i < 7; i++) {
        if (strcmp(prefix, validPrefixes[i]) == 0) {
            return 1;
        }
    }

    printf("Mobile number must start with 018, 019, 017, 013, 014, 015, or 016.\n");
    return 0;
}
