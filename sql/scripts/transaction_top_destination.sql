SELECT DISTINCT
    transaction.to, COUNT(transaction.to) as txcount
FROM
    iconation.transaction
GROUP BY transaction.to
ORDER BY txcount DESC