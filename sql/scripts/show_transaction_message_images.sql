SELECT 
    transaction, concat("https://tracker.icon.foundation/transaction/", hash), data
FROM
    iconation.transaction_message
        JOIN
    iconation.transaction
WHERE
    iconation.transaction_message.transaction = iconation.transaction.id
    and CAST(UNHEX(SUBSTR(data, 3)) AS CHAR(500000)) LIKE "data:image/%"
ORDER BY transaction DESC