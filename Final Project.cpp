#include "framework.h"
#include "Final Project.h"

#include <pqxx/pqxx>
#include "Database.h"
#include<iostream>

// ImGui Headers
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>


#include <iostream>
#include <string>
#include <vector>

using namespace std;


// ============================================================
//                    DOMAIN CLASSES
// ============================================================

// Person
class Person {
    // ...
protected:

    int id;
    string username;
    string passwordHash;

public:
    //default constructor
    Person() :id(0), username(""), passwordHash("") {

    }
    //parameterized constructor
    Person(int id, const string& username, const string& passwordHash)
        :id(id), username(username), passwordHash(passwordHash) {

    }

    int getId() const {
        return id;
    }
    string getuserName() const {
        return username;
    }
    bool verifyPassword(const string & enteredPassword) const {
        return passwordHash == enteredPassword;

    }
    virtual bool login(string username, string password) = 0;

    //destructor
    virtual ~Person() = default;
};


// User class
class User : public Person {
    // ...
private:
    string email;
public:
    //default constructor
    User() :Person(), email(""){}

    //parameterized constructor
    User(int id, const string& username, const string& passwordHash,const string & email)
        :Person(id,username,passwordHash),email(email){}


    string getEmail()const {
        return email;
    }

    bool registerAccount() {
        string query = "INSERT INTO users (username , password_hash, email) VALUES ('"
            + username + "','"
            + passwordHash + "','"
            + email + "');";

        return Database::getInstance().executeNonSelect(query);

    }

    bool upadteProfile(const string& newUserName, const string& newEmail) {
        string query = "UPDATE users SET username = '" + newUserName
            + "', email = '" + newEmail
            + "' WHERE id = " + to_string(id) + ";";

        bool success = Database::getInstance().executeNonSelect(query);
        
        if (success) {
            this->username = newUserName; //update current object
            this->email = newEmail;
        }
        return success;
    }

    bool changePassword(const string& oldPassword, const string& newPassword) {

        if (!verifyPassword(oldPassword)) {
            return false;
        }
        string query = "UPDATE users SET password_hash = '" + newPassword
            + "' WHERE id = " + to_string(id) + ";";

        bool success = Database::getInstance().executeNonSelect(query);

        if (success) {
            this->passwordHash = newPassword; //update current object
        }
        return success;
    }

    bool login(string userName, string enteredPassword) override {

        string query = "SELECT id, username, password_hash, email FROM users WHERE username = '"
            + userName + "';";

        pqxx::result result=Database::getInstance().executeQuery(query);

        //check if username exists in database or not
        if (result.empty()) {
            return false;
        }

        auto row = result[0];
        string storedPassword = row["password_hash"].as<string>();

        //checks if the user entered his password correctly
        if (storedPassword != enteredPassword) {
            return false;
        }

        this->id = row["id"].as<int>();
        this->username = row["username"].as<string>();
        this->passwordHash = storedPassword;
        this->email = row["email"].as<string>();

        return true;
    }

};





// Category
class Category {
private:
    int id;
    string name;

public:
    Category() : id(0), name("") {}

   
    Category(int id, string name)
        : id(id), name(name) {
    }

    int getId() const {
        return id;
    }

    string getName() const {
        return name;
    }

    void setId(int id) {
        this->id = id;
    }

    void setName(string name) {
        this->name = name;
    }
};

// Item
class Item {
protected:

    int id;
    Category category;
    string color;
    string location;
    string date;
    string imagePath;
    string status;

    Item(
        int id,
        Category category,
        string color,
        string location,
        string date,
        string imagePath,
        string status
    )
        : id(id),
        category(category),
        color(color),
        location(location),
        date(date),
        imagePath(imagePath),
        status(status)
    {
    }

public:

    int getId() const {
        return id;
    }

    Category getCategory() const {
        return category;
    }

    string getColor() const {
        return color;
    }

    string getLocation() const {
        return location;
    }

    string getDate() const {
        return date;
    }

    string getImagePath() const {
        return imagePath;
    }

    string getStatus() const {
        return status;
    }

    void setCategory(Category category) {
        this->category = category;
    }

    void setColor(string color) {
        this->color = color;
    }

    void setLocation(string location) {
        this->location = location;
    }

    void setDate(string date) {
        this->date = date;
    }

    void setImagePath(string imagePath) {
        this->imagePath = imagePath;
    }

    void setStatus(string status) {
        this->status = status;
    }

    virtual string getDetails() = 0;

    virtual ~Item() = default;
};



// LostReport
class LostReport : public Item {
    // ...
private:

    int userId;
    string privateDescription;

public:

    LostReport(
        int id,
        int userId,
        Category category,
        string color,
        string location,
        string date,
        string privateDescription,
        string imagePath,
        string status
    )
        :Item(id, category, color, location, date, imagePath, status),
        userId(userId),
        privateDescription(privateDescription) {

    }

    int getUserId() const {
        return userId;
    }
    string getPrivateDescription() const {
        return privateDescription; 
    }


public:

    bool saveToDb()
    {

      

        string query =
        "INSERT INTO lost_reports "
        "(user_id, category_id, color, location, date_lost, "
        "private_description, image_path, status) "
        "VALUES (" +
        to_string(userId) +", "+
        to_string(category.getId()) +", '"+
        color+ "', '"+
        location + "', '"+
        date +"', '"+
        privateDescription +"', '"+
        imagePath +"', '"+
        status + "')";

       bool isSaved = Database::getInstance().executeNonSelect(query);

     

    return isSaved;
    }

    bool updateReport(int currentuser)
    {
        if(currentuser != userId || status == "Returned")
        {
            return false;
        }
        else 
        {
        string query =
        "UPDATE lost_reports SET "
        "category_id = " +to_string(category.getId()) + ", " +
        "color = '"+color + "', " +
        "location = '"+location+"', " +
        "date_lost = '"+ date + "', " +
        "private_description = '" + privateDescription + "', " +
        "image_path = '"+imagePath + "', " +
        "status = '"+status + "' "
        "WHERE id = "+to_string(id);

        return Database::getInstance().executeNonSelect(query);
        }
    }

    bool deleteReport(int currentuser)
    {
        if(currentuser == userId)
        {
        string query =
        "DELETE FROM lost_reports "
        "WHERE id = " + to_string(id);

        return Database::getInstance().executeNonSelect(query);
        }
        else
        {
            return false;
        }
    }

    string getPrivateDescription()
    {
        return privateDescription;
    }

    string getDetails() override
    {
        string details =
            "Category: " + category.getName() + "\n" +
            "Color: " + color + "\n" +
            "Location: " + location + "\n" +
            "Date Lost: " + date + "\n" +
            "Status: " + status + "\n" +
            "Image: " + imagePath + "\n";

        return details;
    }

};


// FoundReport
class FoundReport : public Item {
private:
    int adminId;
    string publicDescription;
    string privateNotes;

public:
    // Default Constructor
    FoundReport()
        : Item(0, Category(), "", "", "", "", "Found"),
        adminId(0),
        publicDescription(""),
        privateNotes("") {
    }

    // Main Constructor
    FoundReport(
        int id,
        int adminId,
        Category category,
        string color,
        string location,
        string date,
        string publicDescription,
        string privateNotes,
        string imagePath,
        string status
    )
        : Item(id, category, color, location, date, imagePath, status),
        adminId(adminId),
        publicDescription(publicDescription),
        privateNotes(privateNotes)
    {
    }

    // Getters and Setters
    int getAdminId() const { return adminId; }
    void setAdminId(int id) { adminId = id; }

    string getPublicDescription() const { return publicDescription; }
    void setPublicDescription(const string& desc) { publicDescription = desc; }

    string getPrivateNotes() const { return privateNotes; }
    void setPrivateNotes(const string& notes) { privateNotes = notes; }

    // Overridden getDetails from Item
    string getDetails() override {
        return getDetails(false);
    }

    // Overloaded getDetails with Admin visibility check
    string getDetails(bool isAdmin) {
        string details =
            "Found Report ID: " + to_string(id) + "\n" +
            "Category: " + category.getName() + "\n" +
            "Color: " + color + "\n" +
            "Location: " + location + "\n" +
            "Date Found: " + date + "\n" +
            "Public Description: " + publicDescription + "\n" +
            "Status: " + status + "\n" +
            "Image: " + imagePath + "\n";

        if (isAdmin) {
            details += "Private Notes: " + privateNotes + "\n";
        }

        return details;
public:
    bool saveToDb();
    bool updateReport();
    string getPrivateNotes();
    string getDetails() override {
          
    }
    Category getCategory() const{
        return category;
    }

    // Database Methods
    bool saveToDb() {
        string query =
            "INSERT INTO found_reports "
            "(admin_id, category_id, color, location, date_found, "
            "public_description, private_notes, image_path, status) "
            "VALUES (" +
            to_string(adminId) + ", " +
            to_string(category.getId()) + ", '" +
            color + "', '" +
            location + "', '" +
            date + "', '" +
            publicDescription + "', '" +
            privateNotes + "', '" +
            imagePath + "', '" +
            status + "')";

        return Database::getInstance().executeNonSelect(query);
    }

    bool updateReport(int currentAdminId) {
        if (currentAdminId != adminId || status == "Returned") {
            return false;
        }

        string query =
            "UPDATE found_reports SET "
            "category_id = " + to_string(category.getId()) + ", " +
            "color = '" + color + "', " +
            "location = '" + location + "', " +
            "date_found = '" + date + "', " +
            "public_description = '" + publicDescription + "', " +
            "private_notes = '" + privateNotes + "', " +
            "image_path = '" + imagePath + "', " +
            "status = '" + status + "' " +
            "WHERE id = " + to_string(id);

        return Database::getInstance().executeNonSelect(query);
    }

    bool deleteReport(int currentAdminId) {
        if (currentAdminId != adminId) {
            return false;
        }

        string query = "DELETE FROM found_reports WHERE id = " + to_string(id);
        return Database::getInstance().executeNonSelect(query);
    }

    // Static Fetch Helper
    static vector<FoundReport> getAllReports() {
        vector<FoundReport> reports;
        string query =
            "SELECT fr.id, fr.admin_id, fr.category_id, c.name AS category_name, "
            "fr.color, fr.location, fr.date_found, fr.public_description, "
            "fr.private_notes, fr.image_path, fr.status "
            "FROM found_reports fr "
            "LEFT JOIN categories c ON fr.category_id = c.id "
            "ORDER BY fr.id DESC;";

        pqxx::result r = Database::getInstance().executeQuery(query);

        for (const auto& row : r) {
            int id = row["id"].as<int>();
            int adminId = row["admin_id"].as<int>();
            int categoryId = row["category_id"].as<int>();
            string categoryName = row["category_name"].is_null() ? "" : row["category_name"].as<string>();

            Category cat(categoryId, categoryName);

            reports.emplace_back(
                id,
                adminId,
                cat,
                row["color"].as<string>(),
                row["location"].as<string>(),
                row["date_found"].as<string>(),
                row["public_description"].as<string>(),
                row["private_notes"].as<string>(),
                row["image_path"].as<string>(),
                row["status"].as<string>()
            );
        }
        return reports;
    }
};

// PossibleMatch
class PossibleMatch {
    // ...

private:

    int id;
    int lostReportId;
    int foundReportId;
    int matchScore;
    string status; 

public:

     PossibleMatch(int lostId, int foundId, int score)
        : id(0),
          lostReportId(lostId),
          foundReportId(foundId),
          matchScore(score),
          status("Pending")
    {
    }

    int getId() const {
        return id;
    }

    int getLostReportId() const {
        return lostReportId;
    }

    int getFoundReportId() const {
        return foundReportId;
    }

    int getMatchScore() const {
        return matchScore;
    }

    string getStatus() const {
        return status;
    }

    void setStatus(const string& newStatus) {
        status = newStatus;
    }

    bool saveToDb() {
    string query = "INSERT INTO possible_matches (lost_report_id, found_report_id, match_score, status) VALUES ("
                 + to_string(lostReportId) + ", "
                 + to_string(foundReportId) + ", "
                 + to_string(matchScore) + ", '"
                 + status + "');";

    return Database::getInstance().executeNonSelect(query);
}


};




// ClaimRequest
class ClaimRequest {
    // ...
private:

    int id;
    int userId;
    int matchId;
    string identifyingDetails;
    string status;   
    int reviewedByAdminId;

public:

    ClaimRequest(int userId,int matchId,const string &identifyingDetails):
        id(0),userId(userId),matchId(matchId),identifyingDetails(identifyingDetails),status("pending"),reviewedByAdminId(0){
    }

    ClaimRequest(int id, int userId, int matchId, const string& identifyingDetails, const string& status, int reviewedByAdminId)
        : id(id), userId(userId), matchId(matchId), identifyingDetails(identifyingDetails), status(status), reviewedByAdminId(reviewedByAdminId) {
    }

    // Getters
    int getId() const { return id; }
    int getUserId() const { return userId; }
    int getMatchId() const { return matchId; }
    string getIdentifyingDetails() const { return identifyingDetails; }
    string getStatus() const { return status; }
    int getReviewedByAdminId() const { return reviewedByAdminId; }

    bool submitClaim() {

        string query = "INSERT INTO claim_requests (user_id, match_id, identifying_details, status) VALUES ("
            + to_string(userId) + ", "
            + to_string(matchId) + ", '"
            + identifyingDetails + "', '"
            + status + "');";

        return Database::getInstance().executeNonSelect(query);
    }

    bool updateStatus(string newStatus) {
        string query = "UPDATE claim_requests SET status = '" + newStatus + "' WHERE id = " + to_string(id) + ";";

        bool success = Database::getInstance().executeNonSelect(query);

        if (success) {
            this->status = newStatus; //update current object's status
        }
        return success;
    }
};


//Notification 
class Notification {
private:
    int id;
    int userId;
    string message;
    string type;
    bool isRead;
public:
    Notification(int id,int userId,const string & message,const string & type,bool isRead)
        :id(id),userId(userId),message(message),type(type),isRead(isRead){}
public:
    int getId()const {
        return id;
    }
    int getUserId() const{
        return userId;
    }
    string getMessage()const {
        return message;
    }
    string getType() const {
        return type;
    }
    bool getIsRead()const {
        return isRead;
    }
};

// NotificationService
class NotificationService {
    // ...
public:

    bool sendNotification(int userid, const string& msg, const string& type) {

        string query = "INSERT INTO notifications (user_id,message,type) VALUES ("
            + to_string(userid) + ",'"
            + msg + "','"
            + type + "');";

        return Database::getInstance().executeNonSelect(query);
    }

    vector<Notification> getUserNotifications(int userid) {

        vector<Notification> userNotifications;

        string query = "SELECT id,user_id,message,type,is_read FROM notifications WHERE user_id=" + to_string(userid) + ";";
        pqxx::result result = Database::getInstance().executeQuery(query);

        for (const auto& row : result) {
            int id = row["id"].as<int>();
            int uid = row["user_id"].as<int>();
            string msg = row["message"].as<string>();
            string type = row["type"].as<string>();
            bool isRead = row["is_read"].as<bool>();

            Notification notify(id, uid, msg, type, isRead);
            userNotifications.push_back(notify);
        }
        return userNotifications;
    }

    bool markAsRead(int notificationId) {
        string query = "UPDATE notifications SET is_read=TRUE WHERE id=" + to_string(notificationId) + ";";
        return Database::getInstance().executeNonSelect(query);
    }

};



// MatchingEngine
class MatchingEngine {
    // ...

private:

    int calculateScore(const LostReport& lost, const FoundReport& found) {
        int score = 0;

        if (lost.getCategory().getName() == found.getCategory().getName()) {
            score += 30;
        }
        if (!lost.getColor().empty() && lost.getColor() == found.getColor()) {
            score += 20;
        }
        if (!lost.getLocation().empty() && lost.getLocation() == found.getLocation()) {
            score += 25;
        }
        if (!lost.getDate().empty() && lost.getDate() == found.getDate()) {
            score += 25;
        }
        return score;
    }
public:

    void runMatchForLostReport(const LostReport& report) {


        string query = "SELECT id, admin_id, category_id, color, location_found, date_found, "
            "public_description, private_notes, image_path, status "
            "FROM found_reports WHERE status = 'found';";

        pqxx::result results = Database::getInstance().executeQuery(query);

        for (const auto& row : results) {
            int foundId = row["id"].as<int>();
            int adminId = row["admin_id"].as<int>();
            int categoryId = row["category_id"].as<int>();
            string color = row["color"].as<string>();
            string location = row["location_found"].as<string>();
            string date = row["date_found"].as<string>();
            string pubDesc = row["public_description"].as<string>();
            string privNotes = row["private_notes"].as<string>();
            string imgPath = row["image_path"].as<string>();
            string status = row["status"].as<string>();


            Category cat(categoryId, "");
            FoundReport found(foundId, adminId, cat, color, location, date, pubDesc, privNotes, imgPath, status);

            int score = calculateScore(report, found);

            if (score >= 70) {
                PossibleMatch match(report.getId(), found.getId(), score);
                if (match.saveToDb()) {
                    NotificationService notifService;
                    string msg = "Match found for your lost report #" + to_string(report.getId()) +
                        " (" + report.getCategory().getName() + ") with score " + to_string(score) + "%!";
                    notifService.sendNotification(report.getUserId(), msg, "match_found");
                }
            }
        }
    }


    void runMatchForFoundReport(const FoundReport& report) {


        string query = "SELECT id, user_id, category_id, color, location, date_lost, "
            "private_description, image_path, status "
            "FROM lost_reports WHERE status = 'lost';";

        pqxx::result results = Database::getInstance().executeQuery(query);

        for (const auto& row : results) {
            int lostId = row["id"].as<int>();
            int userId = row["user_id"].as<int>();
            int categoryId = row["category_id"].as<int>();
            string color = row["color"].as<string>();
            string location = row["location"].as<string>();
            string date = row["date_lost"].as<string>();
            string privDesc = row["private_description"].as<string>();
            string imgPath = row["image_path"].as<string>();
            string status = row["status"].as<string>();

            Category cat(categoryId, "");
            LostReport lost(lostId, userId, cat, color, location, date, privDesc, imgPath, status);

            int score = calculateScore(lost, report);

            if (score >= 70) {
                PossibleMatch match(lost.getId(), report.getId(), score);
                if (match.saveToDb()) {

                    NotificationService notifService;
                    string msg = "Match found for your lost report #" + to_string(lost.getId()) +
                        " (" + lost.getCategory().getName() + ") with score " + to_string(score) + "%!";

                    notifService.sendNotification(userId, msg, "match_found");
                }

            }
        }
    }


};



////***Admin ****/////
struct FoundReportRow {
    int    id = 0;
    string category, color, location, dateFound;
    string publicDescription, privateNotes, imagePath, status;
};

struct ClaimRow {
    int    claimId = 0, userId = 0, matchId = 0;
    int    lostReportId = 0, foundReportId = 0, matchScore = 0;
    string username;
    string identifyingDetails;
    string lostPrivateDescription;
    string foundPublicDescription;
    string foundPrivateNotes;
    string status;
};


//Admin class
class Admin : public Person {
private:
    bool   loggedIn;
    string lastMessage;                 // text the GUI can show after an action
    NotificationService notifier;
    MatchingEngine      matcher;

    //  helpers 
    static string esc(const string& s)  // escape single quotes (basic SQL-injection guard)
    {
        string out;
        for (char c : s) { if (c == '\'') out += "''"; else out += c; }
        return out;
    }
    static string lower(string s)
    {
        for (char& c : s) c = (char)tolower((unsigned char)c);
        return s;
    }
    bool requireLogin()
    {
        if (!loggedIn) { lastMessage = "Admin must be logged in."; return false; }
        return true;
    }
    struct ClaimContext {
        int id = 0, userId = 0, matchId = 0, lostId = 0, foundId = 0;
        string details, status;
    };

    bool loadClaim(int claimId, ClaimContext& ctx)
    {
        pqxx::result r = Database::getInstance().executeQuery(
            "SELECT cr.id, cr.user_id, cr.match_id, cr.identifying_details, cr.status, "
            "pm.lost_report_id, pm.found_report_id "
            "FROM claim_requests cr "
            "JOIN possible_matches pm ON pm.id = cr.match_id "
            "WHERE cr.id = " + to_string(claimId));

        if (r.empty()) { lastMessage = "Claim not found."; return false; }

        ctx.id = r[0]["id"].as<int>();
        ctx.userId = r[0]["user_id"].as<int>();
        ctx.matchId = r[0]["match_id"].as<int>();
        ctx.lostId = r[0]["lost_report_id"].as<int>();
        ctx.foundId = r[0]["found_report_id"].as<int>();
        ctx.details = r[0]["identifying_details"].as<string>();
        ctx.status = lower(r[0]["status"].as<string>());
        return true;
    }
    bool reviewClaim(int claimId, const string& newStatus, const string& matchStatus,
        const string& notifType, bool isApproval)
    {
        if (!requireLogin()) return false;

        ClaimContext c;
        if (!loadClaim(claimId, c)) return false;

        if (c.status != "pending") {
            lastMessage = "This claim has already been processed.";
            return false;
        }
        ClaimRequest claim(c.id, c.userId, c.matchId, c.details, c.status, id);
        if (!claim.updateStatus(newStatus)) {
            lastMessage = "Database error while updating the claim.";
            return false;
        }
        Database::getInstance().executeNonSelect(
            "UPDATE claim_requests SET reviewed_by = " + to_string(id) +
            " WHERE id = " + to_string(c.id));

        Database::getInstance().executeNonSelect(
            "UPDATE possible_matches SET status = '" + matchStatus +
            "' WHERE id = " + to_string(c.matchId));
        string msg;
        if (isApproval) {
            msg = "Your claim has been approved.";

        }
        else {
            msg = "Your claim has been rejected.";
        }
        notifier.sendNotification(c.userId, msg, notifType);

        lastMessage = isApproval ? "Claim approved and user notified."
            : "Claim rejected and user notified.";
        return true;
    }
public:
    //  constructor 
    Admin() : loggedIn(false) { id = 0; }

    string getLastMessage() const { return lastMessage; }
    bool   isLoggedIn()     const { return loggedIn; }

    //  Authentication 
    bool login(string userName, string password) override
    {
        try {
            pqxx::result r = Database::getInstance().executeQuery(
                "SELECT id, username, password_hash FROM admins "
                "WHERE username = '" + esc(userName) + "'");
            if (r.empty()) { lastMessage = "Invalid admin username or password."; return false; }
            id = r[0]["id"].as<int>();
            username = r[0]["username"].as<string>();
            passwordHash = r[0]["password_hash"].as<string>();
            if (!verifyPassword(password)) {          // Person::verifyPassword
                id = 0; username.clear(); passwordHash.clear();
                lastMessage = "Invalid admin username or password.";
                return false;
            }
        }
        catch (const exception& e) { lastMessage = e.what(); return false; }
        loggedIn = true;
        lastMessage = "Admin login successful.";
        return true;
    }
    void logout()
    {
        loggedIn = false;
        id = 0; username.clear(); passwordHash.clear();
        lastMessage = "Logged out.";
    }
    //  Found Report Management
    bool createFoundReport(FoundReport& report)
    {
        if (!requireLogin()) return false;
        report.setStatus("found");
        if (!report.saveToDb()) { lastMessage = "Could not save the found report."; return false; }

        matcher.runMatchForFoundReport(report);
        lastMessage = "Found report created.";
        return true;
    }
    bool editFoundReport(FoundReport& report)
    {
        if (!requireLogin()) return false;
        pqxx::result r = Database::getInstance().executeQuery(
            "SELECT status FROM found_reports WHERE id = " + to_string(report.getId()));
        if (r.empty()) { lastMessage = "Found report not found."; return false; }
        if (lower(r[0][0].as<string>()) == "returned") {
            lastMessage = "A returned item can no longer be edited.";
            return false;
        }
        if (!report.updateReport()) { lastMessage = "Could not update the found report."; return false; }
        matcher.runMatchForFoundReport(report);   // details changed -> re-match
        lastMessage = "Found report updated.";
        return true;
    }

    bool deleteFoundReport(int reportId)
    {
        if (!requireLogin()) return false;

        pqxx::result r = Database::getInstance().executeQuery(
            "SELECT 1 FROM found_reports WHERE id = " + to_string(reportId));
        if (r.empty()) { lastMessage = "Found report not found."; return false; }
        pqxx::result c = Database::getInstance().executeQuery(
            "SELECT 1 FROM claim_requests cr JOIN possible_matches pm ON pm.id = cr.match_id "
            "WHERE pm.found_report_id = " + to_string(reportId) + " LIMIT 1");
        if (!c.empty()) {
            lastMessage = "This item has claims and cannot be deleted.";
            return false;
        }
        bool ok = Database::getInstance().executeNonSelect(
            "DELETE FROM found_reports WHERE id = " + to_string(reportId));
        lastMessage = ok ? "Found report deleted." : "Database error while deleting.";
        return ok;
    }
    vector<FoundReportRow> viewAllFoundReports()
    {
        vector<FoundReportRow> rows;
        if (!requireLogin()) return rows;
        pqxx::result r = Database::getInstance().executeQuery(
            "SELECT f.id, COALESCE(c.name,'') AS category, f.color, f.location_found AS location, "
            "f.date_found::text AS date_found, COALESCE(f.public_description,'') AS public_description, "
            "COALESCE(f.private_notes,'') AS private_notes, COALESCE(f.image_path,'') AS image_path, "
            "f.status FROM found_reports f LEFT JOIN categories c ON c.id = f.category_id "
            "ORDER BY f.id DESC");
        for (const auto& row : r) {
            FoundReportRow f;
            f.id = row["id"].as<int>();
            f.category = row["category"].as<string>();
            f.color = row["color"].as<string>();
            f.location = row["location"].as<string>();
            f.dateFound = row["date_found"].as<string>();
            f.publicDescription = row["public_description"].as<string>();
            f.privateNotes = row["private_notes"].as<string>();
            f.imagePath = row["image_path"].as<string>();
            f.status = row["status"].as<string>();
            rows.push_back(f);
        }
        return rows;
    }
    //   Claim Management (Admin)
    vector<ClaimRow> viewClaimsByStatus(const string& status)
    {
        vector<ClaimRow> rows;
        if (!requireLogin()) return rows;
        pqxx::result r = Database::getInstance().executeQuery(
            "SELECT cr.id AS claim_id, cr.user_id, u.username, cr.match_id, "
            " cr.identifying_details ,cr.status, pm.lost_report_id, pm.found_report_id, "
            "pm.match_score, COALESCE(lr.private_description,'') AS lost_private, "
            "COALESCE(fr.public_description,'') AS found_public, "
            "COALESCE(fr.private_notes,'') AS found_notes "
            "FROM claim_requests cr "
            "JOIN users u ON u.id = cr.user_id "
            "JOIN possible_matches pm ON pm.id = cr.match_id "
            "JOIN lost_reports lr ON lr.id = pm.lost_report_id "
            "JOIN found_reports fr ON fr.id = pm.found_report_id "
            "WHERE LOWER(cr.status) = '" + esc(lower(status)) + "' ORDER BY cr.id");
        for (const auto& row : r) {
            ClaimRow c;
            c.claimId = row["claim_id"].as<int>();
            c.userId = row["user_id"].as<int>();
            c.username = row["username"].as<string>();
            c.matchId = row["match_id"].as<int>();

            c.identifyingDetails = row["identifying_details"].as<string>();

            c.status = row["status"].as<string>();
            c.lostReportId = row["lost_report_id"].as<int>();
            c.foundReportId = row["found_report_id"].as<int>();
            c.matchScore = row["match_score"].as<int>();
            c.lostPrivateDescription = row["lost_private"].as<string>();
            c.foundPublicDescription = row["found_public"].as<string>();
            c.foundPrivateNotes = row["found_notes"].as<string>();
            rows.push_back(c);
        }
        return rows;
    }
    vector<ClaimRow> viewPendingClaims() { return viewClaimsByStatus("pending"); }
    vector<ClaimRow> viewApprovedClaims() { return viewClaimsByStatus("approved"); }
    bool approveClaim(int claimId)
    {
        return reviewClaim(claimId, "approved", "claimed", "claim_approved", true);
    }
    bool rejectClaim(int claimId)
    {
        return reviewClaim(claimId, "rejected", "notified", "claim_rejected", false);
    }
    // Admin clicks "Mark as Returned" after the owner collects the item
    bool markAsReturned(int claimId)
    {
        if (!requireLogin()) return false;
        ClaimContext c;
        if (!loadClaim(claimId, c)) return false;
        if (c.status != "approved") {
            lastMessage = "Only an approved claim can be marked as returned.";
            return false;
        }
        ClaimRequest claim(c.id, c.userId, c.matchId, c.details, c.status, id);
        if (!claim.updateStatus("returned")) {
            lastMessage = "Database error while updating the claim.";
            return false;
        }
        Database::getInstance().executeNonSelect(
            "UPDATE claim_requests SET returned_at = NOW() WHERE id = " + to_string(c.id));
        Database::getInstance().executeNonSelect(
            "UPDATE found_reports SET status = 'returned' WHERE id = " + to_string(c.foundId));
        Database::getInstance().executeNonSelect(
            "UPDATE lost_reports SET status = 'returned' WHERE id = " + to_string(c.lostId));
        lastMessage = "Item marked as returned.";
        return true;
    }
};



// SearchService
class SearchService {
    // ...
public:

    vector<LostReport> searchLostReports(int categoryId, string color, string location);
    vector<FoundReport> searchFoundReports(int categoryId, string color, string location);


};


vector<LostReport> SearchService::searchLostReports(int categoryId, string color, string location) {


    vector<LostReport>  results;
    string query =
        "SELECT * FROM lost_reports "
        "WHERE category_id = " + to_string(categoryId) +
        " AND color = '" + color +
        "' AND location = '" + location + "'";

    pqxx::result r = Database::getInstance().executeQuery(query);

    for (const auto& row : r) {
        int id = row["id"].as<int>();
        int userId = row["user_id"].as<int>();

        int rowCategoryId = row["category_id"].as<int>();
        string rowColor = row["color"].as<string>();
        string rowLocation = row["location"].as<string>();

        string date = row["date_lost"].as<string>();
        string privateDescription = row["private_description"].as<string>();
        string imagePath = row["image_path"].as<string>();
        string status = row["status"].as<string>();


        Category category(rowCategoryId, " ");


        LostReport report(
            id,
            userId,
            category,
            rowColor,
            rowLocation,
            date,
            privateDescription,
            imagePath,
            status
        );

        results.push_back(report);


    }
    return results;

  
    
}


vector<FoundReport>  SearchService::searchFoundReports(int categoryId, string color, string location) {

    vector<FoundReport> results;

    string query=
        "SELECT * FROM found_reports "
        "WHERE category_id = " + to_string(categoryId) +
        " AND color = '" + color +
        "' AND location_found = '" + location + "'";

    pqxx::result r = Database::getInstance().executeQuery(query);

    for (const auto& row : r) {

        int id = row["id"].as<int>();
        int adminId = row["admin_id"].as<int>();

        int rowCategoryId = row["category_id"].as<int>();
        string rowColor = row["color"].as<string>();
        string rowLocation = row["location_found"].as<string>();

        string date = row["date_found"].as<string>();
        string publicDescription = row["public_description"].as<string>();
        string privateNotes = row["private_notes"].as<string>();
        string imagePath = row["image_path"].as<string>();
        string status = row["status"].as<string>();

        Category category(rowCategoryId, " ");

        FoundReport report(
            id,
            adminId,
            category,
            rowColor,
            rowLocation,
            date,
            publicDescription,
            privateNotes,
            imagePath,
            status
        );
        results.push_back(report);

    }
    return results;



}



// AdminDashboard
class AdminDashboard {
    // ...
public:

    int getTotalUsers() {

        pqxx::result r = Database::getInstance().executeQuery("SELECT COUNT(*) FROM users");
        return r[0][0].as<int>();
        


    }
    int getTotalLostItems() {

        pqxx::result r = Database::getInstance().executeQuery("SELECT COUNT(*) FROM lost_reports");
        return r[0][0].as<int>();


    }
    int getTotalFoundItems() {

        pqxx::result r = Database::getInstance().executeQuery("SELECT COUNT(*) FROM found_reports");
        return r[0][0].as<int>();


    }
    int getPendingClaimsCount() {
        pqxx::result r = Database::getInstance().executeQuery(
            "SELECT COUNT(*) FROM claim_requests WHERE status = 'pending'"
        );
        return r[0][0].as<int>();

    }
    int getApprovedClaimsCount() {
        pqxx::result r = Database::getInstance().executeQuery(
            "SELECT COUNT(*) FROM claim_requests WHERE status = 'approved'"
        );
        return r[0][0].as<int>();

    }
    int getReturnedItemsCount() {

        pqxx::result r = Database::getInstance().executeQuery(
            "SELECT COUNT(*) FROM claim_requests WHERE status = 'returned'"
        );
        return r[0][0].as<int>();
    }
    


};


// Forward declarations
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Global Variables for DirectX 11
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



// RENDER FUNCTIONS 

void RenderNotificationsTab(int currentUserId)
{
    NotificationService notificationService;
    vector<Notification> notifications = notificationService.getUserNotifications(currentUserId);

    if (notifications.empty())
    {
        ImGui::Text("No notifications yet.");
    }
    else
    {
        for (auto& n : notifications)
        {
            ImGui::PushID(n.getId());

            if (n.getIsRead())
                ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "%s", n.getMessage().c_str());
            else
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.3f, 1.0f), "%s", n.getMessage().c_str());

            ImGui::SameLine();

            if (!n.getIsRead())
            {
                if (ImGui::Button("Mark as read"))
                {
                    notificationService.markAsRead(n.getId());
                }
            }

            ImGui::Separator();
            ImGui::PopID();
        }
    }

}



void RenderDashboardTab()
{
    AdminDashboard dashboard;

    ImGui::Separator();
    ImGui::Text("Admin Dashboard");

    ImGui ::Text("Total Users: %d", dashboard.getTotalUsers());
    ImGui::Text("Total Lost Items: %d" ,dashboard.getTotalLostItems());
    ImGui::Text("Total Found Items: %d" , dashboard.getTotalFoundItems());
    ImGui::Text("Pending Claims: %d", dashboard.getPendingClaimsCount());
    ImGui::Text("Approved Claims: %d", dashboard.getApprovedClaimsCount());
    ImGui::Text("Returned Items: %d", dashboard.getReturnedItemsCount());
}





void RenderClaimsTab()    ///////// in progress////////
{
    int claimId = 0;

    ImGui::Separator();
    ImGui::Text("Claim Review");

    ImGui::InputInt("Claim ID", &claimId);

    if (ImGui::Button("Approve"))
    {
        // approve claim
    }

    ImGui::SameLine();

    if (ImGui::Button("Reject"))
    {
        // reject claim
    }
}




/////*******MAIN*******///////////

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
  
    

    
    // 1. Create Application Window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Class", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Main Application", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // 2. Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // 3. Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // --- تكبير حجم الخط لجميع عناصر الواجهة ---
    io.FontGlobalScale = 1.5f;

    // Setup Style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);







    // ***********Main Loop*************///
   
    int currentPage = 0;

    //claimRequest

    int claimMatchId = 0;
    char claimDetails[1000] = " ";
    int currentUserId = 1;


    //SearchService
    int searchCategoryId = 0;
    char searchColor[100] = " ";
    char searchLocation[100] = " ";
    vector<LostReport> lostResults;
    vector<FoundReport> foundResults;
   
    

    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start ImGui Frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        //RENDR 

        // --- Application Window UI ---
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        ImGui::Begin("Dashboard");

        ImGui::Text("Welcome to the System Dashboard!");
        ImGui::Separator();

        if (ImGui::Button("Lost Item"))
        {
            currentPage = 1;
        }

        if (ImGui::Button("Found Item"))
        {
            currentPage = 2;
        }

        if (ImGui::Button("Search"))   //DONE
        {
            currentPage = 3;
        }

        if (ImGui::Button("Notifications"))   //DONE
        {
            currentPage = 4;
        }

        if (ImGui::Button("Claim Request"))
        {
            currentPage = 5;
        }


        // Dashboard
        if (currentPage == 0)
        {
            RenderDashboardTab();
        }


        if (currentPage == 1)
        {
            ImGui::Separator();
            ImGui::Text("Report a Lost Item");

            static int lostCategoryId = 0;
            static char lostColor[100] = "";
            static char lostLocation[100] = "";
            static char lostDate[20] = "";
            static char lostPrivateDesc[500] = "";
            static char lostImagePath[200] = "";

            ImGui::InputInt("Category ID", &lostCategoryId);
            ImGui::InputText("Color", lostColor, IM_ARRAYSIZE(lostColor));
            ImGui::InputText("Location", lostLocation, IM_ARRAYSIZE(lostLocation));
            ImGui::InputText("Date (YYYY-MM-DD)", lostDate, IM_ARRAYSIZE(lostDate));
            ImGui::InputTextMultiline("Private Description", lostPrivateDesc, IM_ARRAYSIZE(lostPrivateDesc));
            ImGui::InputText("Image Path (optional)", lostImagePath, IM_ARRAYSIZE(lostImagePath));


            if (ImGui::Button("Submit Lost Report"))
            {
                Category category(lostCategoryId, "");
                LostReport report(
                    0,                 
                    currentUserId,          
                    category,
                    string(lostColor),
                    string(lostLocation),
                    string(lostDate),
                    string(lostPrivateDesc),
                    string(lostImagePath),
                    "lost"                  // status
                );

                if (report.saveToDb())
                {
                    MatchingEngine engine;             
                    engine.runMatchForLostReport(report);

                    ImGui::Text("Report saved successfully!");
                }
                else
                {
                    ImGui::Text("Failed to save report.");
                }
            }
        }
    

        if (currentPage == 2)
        {
            ImGui::Separator();  //line
            ImGui::Text("Found Item Page");
        }

        if (currentPage == 3)
        {
            ImGui::Separator();
            ImGui::Text("Search");

            ImGui::InputInt("Category ID", &searchCategoryId);

            ImGui::InputText("Color ", searchColor, IM_ARRAYSIZE(searchColor));

            ImGui::InputText("Location", searchLocation, IM_ARRAYSIZE(searchLocation));

            if (ImGui::Button("search lost ")) {

                SearchService  searchService;
                lostResults = searchService.searchLostReports(

                    searchCategoryId,
                    string(searchColor),
                    string(searchLocation)
                );
            }

            if (ImGui::Button("search found ")) {

                SearchService  searchService;
                foundResults = searchService.searchFoundReports(

                    searchCategoryId,
                    string(searchColor),
                    string(searchLocation)
                );
            }


        }
    

        if (currentPage == 4)
        {

            RenderNotificationsTab(1);

        }

        if (currentPage == 5)
        {
            ImGui::Separator();
            ImGui::Text("Claim Request");

            ImGui::InputInt("Match ID", &claimMatchId);

            ImGui::InputTextMultiline(
                "Identifying Details",
                claimDetails,
                IM_ARRAYSIZE(claimDetails),  //array size 1000
                ImVec2(500, 120));  // // Width and height of the input box




                if (ImGui::Button("Submit Claim")) {
                    ClaimRequest claim(currentUserId, claimMatchId, string(claimDetails));

                    claim.submitClaim();

                }


        }

        if (currentPage == 6) {

            RenderClaimsTab();
        }

        

        ImGui::End();

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions for DirectX 11
bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}