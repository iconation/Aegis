SELECT 
    transaction, concat("https://tracker.icon.foundation/transaction/", hash), CAST(UNHEX(SUBSTR(data, 3)) AS CHAR(500000))
FROM
    iconation.transaction_message
        JOIN
    iconation.transaction
WHERE
    iconation.transaction_message.transaction = iconation.transaction.id
    and CAST(UNHEX(SUBSTR(data, 3)) AS CHAR(500000)) != "icxgames game divide bet"
    and CAST(UNHEX(SUBSTR(data, 3)) AS CHAR(500000)) != "icxgames reward payout"
    and CAST(UNHEX(SUBSTR(data, 3)) AS CHAR(500000)) != "icxgames lotto divide bet"
    and CAST(UNHEX(SUBSTR(data, 3)) AS CHAR(500000)) != "icxgames lotto return"
    and CAST(UNHEX(SUBSTR(data, 3)) AS CHAR(500000)) NOT LIKE "1USD : %"
    and CAST(UNHEX(SUBSTR(data, 3)) AS CHAR(500000)) NOT LIKE "[Mainnet]%"
    and CAST(UNHEX(SUBSTR(data, 3)) AS CHAR(500000)) NOT LIKE "[UPbit]%"
ORDER BY transaction DESC