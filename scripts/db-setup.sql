--DROP TABLE IF EXISTS current_assets CASCADE;
--DROP TABLE IF EXISTS trade_history CASCADE;

CREATE TABLE IF NOT EXISTS current_assets (
    id SERIAL PRIMARY KEY,
    ticker VARCHAR(50),
    amount INT,
    average_price DECIMAL(10,2)
);

-- todo: add constraints, triggers

CREATE TABLE IF NOT EXISTS trade_history (
    id SERIAL PRIMARY KEY,
    tx_type VARCHAR(10),
	ticker VARCHAR(50),
    amount INT,
    price DECIMAL(10,2),
    date DATE
);