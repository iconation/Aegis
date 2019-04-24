SELECT DISTINCT
    transaction.to, COUNT(transaction.to)
FROM
    iconation.transaction
GROUP BY transaction.to
