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
        scanf("%d", &choice);

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
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 3);

    return 0;
}

// Load users from file
void loadUsers() {
    FILE *file = fopen(FILENAME_USERS, "r");
    if (file == NULL) {
        printf("No user data found. Starting with an empty list.\n");
        return;
    }
    while (fscanf(file, "%s %s %d", users[userCount].username, users[userCount].password, &users[userCount].isAdmin) != EOF) {
        userCount++;
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
    while (fscanf(file, "%s %s %f %d %f %f %[^\n]", products[productCount].name, products[productCount].category, &products[productCount].price, &products[productCount].stock, &products[productCount].discount, &products[productCount].rating, products[productCount].reviews) != EOF) {
        productCount++;
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
        fprintf(file, "%s %s %.2f %d %.2f %.2f %s\n", products[i].name, products[i].category, products[i].price, products[i].stock, products[i].discount, products[i].rating, products[i].reviews);
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
    while (fscanf(file, "%s %s %d %f %s %[^\n]", orders[orderCount].username, orders[orderCount].productName, &orders[orderCount].quantity, &orders[orderCount].totalPrice, orders[orderCount].paymentMethod, orders[orderCount].address) != EOF) {
        orderCount++;
    }
    fclose(file);
}

// Save orders to file
void saveOrders() {
    FILE *file = fopen(FILENAME_ORDERS, "w");
    if (file == NULL) {
        printf("Error saving order data.\n");
        return;
    }
    for (int i = 0; i < orderCount; i++) {
        fprintf(file, "%s %s %d %.2f %s %s\n", orders[i].username, orders[i].productName, orders[i].quantity, orders[i].totalPrice, orders[i].paymentMethod, orders[i].address);
    }
    fclose(file);
}

// Save order history to file
void saveOrderHistory() {
    FILE *file = fopen(FILENAME_ORDER_HISTORY, "a"); // Append mode
    if (file == NULL) {
        printf("Error saving order history.\n");
        return;
    }
    for (int i = 0; i < orderCount; i++) {
        fprintf(file, "User: %s, Product: %s, Qty: %d, Total: %.2f, Method: %s, Address: %s\n",
                orders[i].username, orders[i].productName, orders[i].quantity,
                orders[i].totalPrice, orders[i].paymentMethod, orders[i].address);
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
    printf("Enter username: ");
    scanf("%s", newUser.username);
    printf("Enter password: ");
    scanf("%s", newUser.password);
    newUser.isAdmin = 0; // Default to regular user
    users[userCount++] = newUser;
    saveUsers();
    printf("User registered successfully!\n");
}

// Login a user
int loginUser(char *username) {
    char password[PASSWORD_LENGTH];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    // Check for fixed admin credentials
    if (strcmp(username, "admin") == 0 && strcmp(password, "MATRF") == 0) {
        printf("Admin login successful!\n");
        return 1; // Return 1 for admin
    }

    // Check for regular users
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            printf("Login successful!\n");
            return 0; // Return 0 for regular user
        }
    }

    printf("Invalid username or password.\n");
    return -1; // Login failed
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
        scanf("%d", &choice);

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
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 6);
}

// User panel
void userPanel(char *username) {
    int choice;
    do {
        printf("\nUser Panel\n");
        printf("1. View Products\n");
        printf("2. Search Products\n");
        printf("3. Add to Cart\n");
        printf("4. Checkout\n");
        printf("5. Review Products\n");
        printf("6. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

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
                checkout(username);
                break;
            case 5:
                provideRatingAndReview(username);
                break;
            case 6:
                printf("Logged out.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 6);
}

// Add a new product (admin only)
void addProduct() {
    if (productCount >= MAX_PRODUCTS) {
        printf("Product limit reached. Cannot add more products.\n");
        return;
    }
    Product newProduct;
    printf("Enter product name: ");
    scanf("%s", newProduct.name);
    printf("Enter product category: ");
    scanf("%s", newProduct.category);
    printf("Enter product price: ");
    scanf("%f", &newProduct.price);
    printf("Enter product stock: ");
    scanf("%d", &newProduct.stock);
    printf("Enter product discount (%%): ");
    scanf("%f", &newProduct.discount);
    newProduct.rating = 0; // Default rating
    strcpy(newProduct.reviews, "No reviews yet."); // Default review
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
    int serial;
    printf("Enter the serial number of the product to delete: ");
    scanf("%d", &serial);

    if (serial < 1 || serial > productCount) {
        printf("Invalid serial number.\n");
        return;
    }

    // Shift all products after the deleted product one position to the left
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
    int serial;
    printf("Enter the serial number of the product to update discount: ");
    scanf("%d", &serial);

    if (serial < 1 || serial > productCount) {
        printf("Invalid serial number.\n");
        return;
    }

    printf("Enter new discount (%%): ");
    scanf("%f", &products[serial - 1].discount);
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
    int choice;
    printf("Search by:\n");
    printf("1. Category\n");
    printf("2. Price Range\n");
    printf("3. Both Category and Price Range\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        char category[50];
        printf("Enter category to search: ");
        scanf("%s", category);
        printf("\nProducts in category '%s':\n", category);
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
            }
        }
    } else if (choice == 2) {
        float minPrice, maxPrice;
        printf("Enter minimum price: ");
        scanf("%f", &minPrice);
        printf("Enter maximum price: ");
        scanf("%f", &maxPrice);
        printf("\nProducts between %.2f and %.2f:\n", minPrice, maxPrice);
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
            }
        }
    } else if (choice == 3) {
        char category[50];
        float minPrice, maxPrice;
        printf("Enter category to search: ");
        scanf("%s", category);
        printf("Enter minimum price: ");
        scanf("%f", &minPrice);
        printf("Enter maximum price: ");
        scanf("%f", &maxPrice);
        printf("\nProducts in category '%s' and between %.2f and %.2f:\n", category, minPrice, maxPrice);
        for (int i = 0; i < productCount; i++) {
            if (strcmp(products[i].category, category) == 0 && products[i].price >= minPrice && products[i].price <= maxPrice) {
                printf("Serial: %d\n", i + 1);
                printf("Name: %s\n", products[i].name);
                printf("Price: %.2f\n", products[i].price);
                printf("Discount: %.2f%%\n", products[i].discount);
                printf("Stock: %d\n", products[i].stock);
                printf("Rating: %.2f\n", products[i].rating);
                printf("Reviews: %s\n", products[i].reviews);
                printf("------------------------\n");
            }
        }
    } else {
        printf("Invalid choice.\n");
    }
}

// Add product to cart
void addToCart(char *username) {
    displayProducts();
    int serial, quantity;
    printf("Enter the serial number of the product to add to cart: ");
    scanf("%d", &serial);
    printf("Enter quantity: ");
    scanf("%d", &quantity);

    if (serial < 1 || serial > productCount) {
        printf("Invalid serial number.\n");
        return;
    }

    if (products[serial - 1].stock >= quantity) {
        Order newOrder;
        strcpy(newOrder.username, username);
        strcpy(newOrder.productName, products[serial - 1].name);
        newOrder.quantity = quantity;
        newOrder.totalPrice = products[serial - 1].price * quantity * (1 - products[serial - 1].discount / 100);
        printf("Enter your address: ");
        getchar(); // Clear buffer
        fgets(newOrder.address, 100, stdin);
        newOrder.address[strcspn(newOrder.address, "\n")] = 0; // Remove newline

        orders[orderCount++] = newOrder;
        saveOrders();
        updateStock(products[serial - 1].name, quantity);
        printf("Product added to cart successfully!\n");
    } else {
        printf("Insufficient stock.\n");
    }
}

// Checkout and place order
void checkout(char *username) {
    float total = 0;
    printf("\nYour Cart:\n");
    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].username, username) == 0) {
            printf("Product: %s, Quantity: %d, Total Price: %.2f\n", orders[i].productName, orders[i].quantity, orders[i].totalPrice);
            total += orders[i].totalPrice;
        }
    }
    printf("Total Amount: %.2f\n", total);

    if (total == 0) {
        printf("Your cart is empty. No payment required.\n");
        getchar(); // Wait for user input
        return;
    }

    // Payment method selection
    printf("Choose payment method:\n");
    printf("1. Visa/Mastercard\n");
    printf("2. Mobile Banking (Bkash/Nagad)\n");
    printf("3. Cash on Delivery\n");
    printf("4. Back to Main Menu\n");
    printf("Enter your choice: ");
    int paymentChoice;
    scanf("%d", &paymentChoice);

    switch (paymentChoice) {
        case 1: {
            char accountNumber[20], pin[10];
            printf("Enter your account number: ");
            scanf("%s", accountNumber);
            printf("Enter your PIN: ");
            scanf("%s", pin);
            printf("Processing payment...\n");
            printf("Payment Succeed!\n");
            printf("Your %.2f Taka Paid\n", total);
            strcpy(orders[orderCount-1].paymentMethod, "Visa/Mastercard");
            break;
        }
        case 2: {
            int mobileChoice;
            printf("Choose mobile banking service:\n");
            printf("1. Bkash\n");
            printf("2. Nagad\n");
            printf("Enter your choice: ");
            scanf("%d", &mobileChoice);

            char mobileNumber[15], pin[10];
            do {
                printf("Enter your mobile number (11 digits): ");
                scanf("%s", mobileNumber);
            } while (!validateMobileNumber(mobileNumber));
            printf("Enter your PIN: ");
            scanf("%s", pin);
            printf("Processing payment...\n");
            printf("Payment Succeed!\n");
            printf("Your %.2f Taka Paid\n", total);
            strcpy(orders[orderCount-1].paymentMethod, (mobileChoice == 1) ? "Bkash" : "Nagad");
            break;
        }
        case 3: {
            printf("You have chosen Cash on Delivery. Payment will be made upon delivery.\n");
            strcpy(orders[orderCount-1].paymentMethod, "Cash on Delivery");
            break;
        }
        case 4: {
            printf("Returning to the main menu...\n");
            return;
        }
        default:
            printf("Invalid choice. Defaulting to Cash on Delivery.\n");
            strcpy(orders[orderCount-1].paymentMethod, "Cash on Delivery");
    }

    // Save to order history and clear cart
    saveOrderHistory();
    clearCart(username);
    printf("Thank you for your purchase!\n");
}

// Update stock after purchase
void updateStock(char *productName, int quantity) {
    for (int i = 0; i < productCount; i++) {
        if (strcmp(products[i].name, productName) == 0) {
            products[i].stock -= quantity;
            if (products[i].stock == 0) {
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
    displayProducts();
    int serial;
    printf("Enter the serial number of the product to review (or 0 to go back): ");
    scanf("%d", &serial);

    if (serial == 0) {
        printf("Returning to the user menu...\n");
        return;
    }

    if (serial < 1 || serial > productCount) {
        printf("Invalid serial number.\n");
        return;
    }

    printf("Enter your rating (0-5): ");
    scanf("%f", &products[serial - 1].rating);
    printf("Enter your review: ");
    getchar(); // Clear buffer
    fgets(products[serial - 1].reviews, 100, stdin);
    products[serial - 1].reviews[strcspn(products[serial - 1].reviews, "\n")] = 0; // Remove newline
    saveProducts();
    printf("Thank you for your feedback!\n");
}

// Clear the user's cart after payment
void clearCart(char *username) {
    int newOrderCount = 0;
    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].username, username) != 0) {
            orders[newOrderCount++] = orders[i];
        }
    }
    orderCount = newOrderCount;
    saveOrders();
}

// Validate mobile number (11 digits, starting with 018/019/017/013/014/015/016)
int validateMobileNumber(char *number) {
    if (strlen(number) != 11) {
        printf("Mobile number must be 11 digits.\n");
        return 0;
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
