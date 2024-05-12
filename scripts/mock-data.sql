INSERT INTO current_assets (ticker, amount, average_price)
VALUES 
    ('AAPL', 100, 150.75),
    ('GOOG', 50, 2500.50),
    ('MSFT', 75, 300.25);

INSERT INTO trade_history (tx_type, ticker, amount, price, date)
VALUES 
    ('BUY', 'AAPL', 50, 155.25, '2024-05-10'),
    ('BUY', 'GOOG', 25, 2480.75, '2024-05-11'),
    ('SELL', 'AAPL', 20, 160.50, '2024-05-12'),
    ('SELL', 'MSFT', 30, 305.00, '2024-05-12');