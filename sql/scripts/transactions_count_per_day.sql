SELECT 
    DATE(FROM_UNIXTIME(block.timestamp DIV 1000000)),
    COUNT(DISTINCT all_transactions.txid) + COUNT(DISTINCT all_transactions.itxid) 
FROM
    iconation.block
LEFT JOIN
    (
		SELECT
            transaction.id as txid,
            internal_transaction.id as itxid,
			transaction.block as blockid
		FROM
			transaction
		LEFT JOIN
			internal_transaction
		ON
			internal_transaction.transaction = iconation.transaction.id
    ) as all_transactions
    ON all_transactions.blockid = iconation.block.id
GROUP BY DATE(FROM_UNIXTIME(block.timestamp DIV 1000000))