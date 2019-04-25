SELECT 
    DATE(FROM_UNIXTIME(block.timestamp DIV 1000000)),
    COUNT(DISTINCT all_transactions.txid) + COUNT(DISTINCT all_transactions.itxid)
FROM
    iconation.block
        LEFT JOIN
    (SELECT 
        transaction.id AS txid,
            internal_transaction.id AS itxid,
            internal_transaction.amount AS itxamount,
            transaction.amount AS txamount,
            transaction.block AS blockid
    FROM
        transaction
    LEFT JOIN internal_transaction ON internal_transaction.transaction = iconation.transaction.id
        AND internal_transaction.token_type = 0) AS all_transactions ON all_transactions.blockid = iconation.block.id
	WHERE
		-- Filter > 1000 ICX Transfers
		all_transactions.itxamount > 1000000000000000000000 or all_transactions.txamount > 1000000000000000000000
GROUP BY DATE(FROM_UNIXTIME(block.timestamp DIV 1000000))