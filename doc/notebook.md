09-03-2026. Started a attempt to start documenting my trail and failuers. Working on fixing OrderBook as I am not able to update n_ask or n_bids. Also finsihing up OrderBook::top(int lvl) right now.

09-03-2026. Increamented the aggregate but never actually store within OrderBook::orders_.

09-04-2026. Working on making Cancel, Delete and Excute work with a unseen order_id. Ran into this problem at order_id 16063194...

09-04-2026. Finsihed doctest for test_order_book_remove. Had issues within the excute part, I was accidently doing 

asks_[v.price_ticks] -= v.size

which removed the whole size and therefore erased the whole map[i]