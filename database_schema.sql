-- ============================================================
-- Smart Lost & Found Management System
-- Database Schema (PostgreSQL)
-- ============================================================
-- This schema matches the class design:
-- Person -> User / Admin
-- Item -> LostReport / FoundReport (HAS-A Category)
-- MatchingEngine -> PossibleMatch
-- ClaimRequest, NotificationService, AdminDashboard, SearchService
-- ============================================================


-- ============================================================
-- 1. USERS & ADMINS  (maps to Person -> User / Admin)
-- ============================================================

CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    email VARCHAR(100) UNIQUE,
    created_at TIMESTAMP DEFAULT NOW()
);

CREATE TABLE admins (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT NOW()
);


-- ============================================================
-- 2. CATEGORIES  (maps to Category class - Composition)
-- ============================================================

CREATE TABLE categories (
    id SERIAL PRIMARY KEY,
    name VARCHAR(50) UNIQUE NOT NULL
);

INSERT INTO categories (name) VALUES
('Wallet'), ('Phone'), ('Keys'), ('Bag'),
('Documents'), ('Electronics'), ('Clothes'), ('Others');


-- ============================================================
-- 3. LOST REPORTS  (maps to LostReport : Item)
-- ============================================================

CREATE TABLE lost_reports (
    id SERIAL PRIMARY KEY,
    user_id INT REFERENCES users(id) ON DELETE CASCADE,
    category_id INT REFERENCES categories(id),
    color VARCHAR(50),
    location VARCHAR(150),
    date_lost DATE,
    private_description TEXT,       -- visible to Admin only
    image_path VARCHAR(255),        -- optional
    status VARCHAR(20) DEFAULT 'lost',  -- lost / matched / returned
    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP DEFAULT NOW()
);


-- ============================================================
-- 4. FOUND REPORTS  (maps to FoundReport : Item)
-- ============================================================

CREATE TABLE found_reports (
    id SERIAL PRIMARY KEY,
    admin_id INT REFERENCES admins(id),
    category_id INT REFERENCES categories(id),
    color VARCHAR(50),
    location_found VARCHAR(150),
    date_found DATE,
    public_description TEXT,
    private_notes TEXT,             -- visible to Admin only
    image_path VARCHAR(255),        -- optional
    status VARCHAR(20) DEFAULT 'found', -- found / matched / returned
    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP DEFAULT NOW()
);


-- ============================================================
-- 5. POSSIBLE MATCHES  (maps to MatchingEngine output / PossibleMatch)
-- ============================================================

CREATE TABLE possible_matches (
    id SERIAL PRIMARY KEY,
    lost_report_id INT REFERENCES lost_reports(id) ON DELETE CASCADE,
    found_report_id INT REFERENCES found_reports(id) ON DELETE CASCADE,
    match_score INT NOT NULL,       -- e.g. 0-100
    status VARCHAR(20) DEFAULT 'pending', -- pending / notified / claimed
    created_at TIMESTAMP DEFAULT NOW()
);


-- ============================================================
-- 6. NOTIFICATIONS  (maps to NotificationService)
-- ============================================================

CREATE TABLE notifications (
    id SERIAL PRIMARY KEY,
    user_id INT REFERENCES users(id) ON DELETE CASCADE,
    message TEXT NOT NULL,
    type VARCHAR(30),  -- match_found / claim_approved / claim_rejected
    is_read BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP DEFAULT NOW()
);


-- ============================================================
-- 7. CLAIM REQUESTS  (maps to ClaimRequest)
-- ============================================================

CREATE TABLE claim_requests (
    id SERIAL PRIMARY KEY,
    user_id INT REFERENCES users(id) ON DELETE CASCADE,
    match_id INT REFERENCES possible_matches(id) ON DELETE CASCADE,
    reason_why_mine TEXT,
    identifying_details TEXT,
    ownership_proof VARCHAR(255),   -- optional (file path)
    status VARCHAR(20) DEFAULT 'pending', -- pending / approved / rejected / returned
    reviewed_by INT REFERENCES admins(id),
    pickup_deadline DATE,
    returned_at TIMESTAMP,
    created_at TIMESTAMP DEFAULT NOW()
);


-- ============================================================
-- USEFUL QUERIES (examples for the team)
-- ============================================================

-- A) Simple match score calculation (text-based matching)
-- Same Category => +30, Same Color => +20, Same Location => +25
-- Run this after inserting a new Lost or Found report:
--
-- SELECT fr.id AS found_id,
--        ( (CASE WHEN fr.category_id = $1 THEN 30 ELSE 0 END)
--        + (CASE WHEN fr.color = $2 THEN 20 ELSE 0 END)
--        + (CASE WHEN fr.location_found = $3 THEN 25 ELSE 0 END) ) AS match_score
-- FROM found_reports fr
-- WHERE fr.status = 'found';

-- B) Admin Dashboard stats
-- SELECT
--   (SELECT COUNT(*) FROM users) AS total_users,
--   (SELECT COUNT(*) FROM lost_reports) AS total_lost_items,
--   (SELECT COUNT(*) FROM found_reports) AS total_found_items,
--   (SELECT COUNT(*) FROM claim_requests WHERE status='pending') AS pending_claims,
--   (SELECT COUNT(*) FROM claim_requests WHERE status='approved') AS approved_claims,
--   (SELECT COUNT(*) FROM claim_requests WHERE status='returned') AS returned_items;

-- C) Search / Filter example
-- SELECT * FROM lost_reports
-- WHERE category_id = $1
--   AND location ILIKE '%' || $2 || '%'
--   AND date_lost BETWEEN $3 AND $4
--   AND status = $5;
