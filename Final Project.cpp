#include "framework.h"
#include "Final Project.h"

#include <pqxx/pqxx>
#include "Database.h"

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
    int getId();
    string getuserName();
    bool verifyPassword(string passwod);
    virtual bool login(string username, string password) = 0;

    virtual ~Person() = default;
};

// User
class User : public Person {
    // ...
private:
    string email;
public:

    bool registerAccount();
    bool upadteProfile( const string& newUserName,const string& newEmail);
    bool changePassword( const string& oldPassword ,const string& newpassword);
    bool login(string userName , string password) override ;

};


// Admin
class Admin : public Person {
 private :
    string email;
    vector<FoundReport> foundReports;
    vector<Claim> claims;
    int nextReportId;
  public:
    // constructor
  Admin(int id, string username, string password) : User(id, username, password)
{
    nextReportId = 1;
}
     // authentication 
bool login(string username, string password) override
{
    
    if (getUsername() == username &&
        getPassword() == password)
    {
        cout << "Admin login successful.\n";
        return true;
    }

    cout << "Invalid admin username or password.\n";
    return false;
}
 // Found report Management
bool createFoundReport(FoundReport report)
{
    report.setReportId(nextReportId);
    report.setStatus("Found");

    foundReports.push_back(report);

    cout << "Found report created successfully.\n";
    cout << "Report ID: " << nextReportId << endl;
    nextReportId++;
    return true;
}
bool editFoundReport(int reportId, FoundReport updatedReport)
{
    int index = findFoundReportIndex(reportId);
    // Report does not exist
    if (index == -1)
    {
        cout << "Found report not found.\n";
        return false;
    }
    updatedReport.setReportId(reportId);

    updatedReport.setStatus(foundReports[index].getStatus());

    foundReports[index] = updatedReport;

    cout << "Found report updated successfully.\n";
    return true;
}
bool deleteFoundReport(int reportId)
{
    int index = findFoundReportIndex(reportId);
    if (index == -1)
    {
        cout << "Found report not found.\n";
        return false;
    }
    foundReports.erase(foundReports.begin() + index);
    cout << "Found report deleted successfully.\n";
    return true;
}
void viewAllFoundReports()
{
    if (foundReports.empty())
    {
        cout << "There are no found reports.\n";
        return;
    }
    cout << "\n";
    cout << "============================================\n";
    cout << "           ALL FOUND REPORTS\n";
    cout << "============================================\n";
    for (const FoundReport& report : foundReports)
    {
        cout << "\n";
        cout << "Report ID: "  << report.getReportId()  << endl;

        cout << "Category: " << report.getCategory() << endl;

        cout << "Color: " << report.getColor()  << endl;

        cout << "Location Found: " << report.getLocation()  << endl;

        cout << "Date Found: " << report.getDateFound() << endl;

        cout << "Public Description: " << report.getPublicDescription() << endl;

        cout << "Private Notes: " << report.getPrivateNotes() << endl;

        cout << "Status: "    << report.getStatus()  << endl;
        cout << "--------------------------------------------\n";
    }
}
  // Claim Management
void viewPendingClaims()
{
    bool foundPendingClaim = false;
    cout << "\n";
    cout << "============================================\n";
    cout << "             PENDING CLAIMS\n";
    cout << "============================================\n";
    for (const Claim& claim : claims)
    {
        if (claim.getStatus() == "Pending")
        {
            foundPendingClaim = true;

            cout << "\n";

            cout << "Claim ID: " << claim.getClaimId() << endl;

            cout << "User ID: " << claim.getUserId() << endl;

            cout << "Found Report ID: "  << claim.getFoundReportId() << endl;

            cout << "Reason: " << claim.getReason() << endl;

            cout << "Identifying Details: " << claim.getIdentifyingDetails() << endl;

            cout << "Ownership Proof: " << claim.getOwnershipProof() << endl;

            cout << "Status: " << claim.getStatus() << endl;
            cout << "--------------------------------------------\n";
        }
    }
    if (!foundPendingClaim)
    {
        cout << "There are no pending claims.\n";
    }
}
bool approveClaim(int claimId)
{
    int index = findClaimIndex(claimId);
    // Claim doesn't exist
    if (index == -1)
    {
        cout << "Claim not found.\n";
        return false;
    }
    // We can only approve Pending claims
    if (claims[index].getStatus() != "Pending")
    {
        cout << "This claim has already been processed.\n";
        return false;
    }
    claims[index].setStatus("Approved");
    cout << "Claim approved successfully.\n";
    cout << "Notification: User can collect the item from the Security Office.\n";
    return true;
}
bool rejectClaim(int claimId)
{
    int index = findClaimIndex(claimId);

    if (index == -1)
    {
        cout << "Claim not found.\n";
        return false;
    }

    // Only Pending claims can be rejected
    if (claims[index].getStatus() != "Pending")
    {
        cout << "This claim has already been processed.\n";
        return false;
    }

    
    claims[index].setStatus("Rejected");

    cout << "Claim rejected successfully.\n";

 cout << "Notification: Your claim has been rejected.\n";
    return true;
}
bool markAsReturned(int foundReportId)
{
    int reportIndex = findFoundReportIndex(foundReportId);

    // Found report doesn't exist
    if (reportIndex == -1)
    {
        cout << "Found report not found.\n";
        return false;
    }

    bool approvedClaimExists = false;

    for (const Claim& claim : claims)
    {
        if (claim.getFoundReportId() == foundReportId &&
            claim.getStatus() == "Approved")
        {
            approvedClaimExists = true;
            break;
        }
    }
    if (!approvedClaimExists)
    {
        cout << "The item cannot be marked as returned.\n";
        cout << "There is no approved claim for this item.\n";

        return false;
    }

    foundReports[reportIndex].setStatus("Returned");

    cout << "Item marked as returned successfully.\n";

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

    int getId() {
        return id;
    }

    string getName() {
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

    int getId() {
        return id;
    }

    Category getCategory() {
        return category;
    }

    string getColor() {
        return color;
    }

    string getLocation() {
        return location;
    }

    string getDate() {
        return date;
    }

    string getImagePath() {
        return imagePath;
    }

    string getStatus() {
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

        return Database::getInstance().executeNonSelect(query);
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
        "Category: "+category.getName()+"\n"+
        "Color: "+color +"\n" +
        "Location: "+location+"\n"+
        "Date Lost: " +date + "\n"+
        "Status: " +status + "\n"+
        "Image: "+imagePath +"\n";

        if (isAdmin)
        {
            details += "Private Description: " + privateDescription + "\n";
        }

        return details;
    }

};


// FoundReport
class FoundReport : public Item {
    // ...
private:

    int adminId;
    string publicDescription;
    string privateNotes;

public:
    bool saveToDb();
    bool updateReport();
    string getPrivateNotes();
    string getDetails() override ;



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
          status("Possible")
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

    bool saveToDb();


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

        return Database::getInstance().executeNonSelect(query);
    }

};

// MatchingEngine
class MatchingEngine {
    // ...

private:

    int calculateScore( const LostReport& lost, const FoundReport& found);

public:

    void runMatchForLostReport( const LostReport& report);
    void runMatchForFoundReport( const FoundReport& report);


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
            +to_string (userid)+",'"
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

// SearchService
class SearchService {
    // ...
public:

    vector<LostReport> searchLostReports(int categoryId, string color, string location);
    vector<FoundReport> searchFoundReports(int categoryId, string color, string location);


};


vector<LostReport> SearchService::searchLostReports(int categoryId, string color, string location) {


    vector<LostReport>  results;


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

    int claimMatchId = 0;
    char claimDetails[1000] = " ";
    int currentUserId = 1;
   
    

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

        if (ImGui::Button("Search"))
        {
            currentPage = 3;
        }

        if (ImGui::Button("Notifications"))
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
            ImGui::Text("Lost Item Page");
        }

        if (currentPage == 2)
        {
            ImGui::Separator();  //line
            ImGui::Text("Found Item Page");
        }

        if (currentPage == 3)
        {
            ImGui::Separator();
            ImGui::Text("Search Page");
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