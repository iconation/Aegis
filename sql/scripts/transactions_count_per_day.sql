SELECT
    DATE(FROM_UNIXTIME(block.timestamp / 1000000)),
    COUNT(DISTINCT id) totalCount
FROM
    iconation.block
GROUP BY DATE(FROM_UNIXTIME(block.timestamp / 1000000))