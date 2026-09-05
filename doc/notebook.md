09-03-2026. Started a attempt to start documenting my trail and failuers. Working on fixing OrderBook as I am not able to update n_ask or n_bids. Also finsihing up OrderBook::top(int lvl) right now.

09-03-2026. Increamented the aggregate but never actually store within OrderBook::orders_.

09-04-2026. Working on making Cancel, Delete and Excute work with a unseen order_id. Ran into this problem at order_id 16063194...

09-04-2026. Finsihed doctest for test_order_book_remove. Had issues within the excute part, I was accidently doing 

asks_[v.price_ticks] -= v.size

which removed the whole size and therefore erased the whole map[i]


09-04-2026. When erasing order_, rather than looking up order_id I was looking up price_ticks, which it wasn't meant for.

09-04-2026. Working on making the OrderBook code easier to read. Also working on having asserts. However getting a lot of errors with asserts and still can't pass 648 with order_id 16216077

09-04-2026. I have found another bug while working on the previous one. Basically the message won't have anything that's too past it's constructed price range. So now i have to create cases for this too.

09-05-2026. I am still working on fixing the same bug, this is my 8th hour on the same bug. 

09-05-2025. I am just finding out that because LOBSTER ignores those prices, there isn't really anything I can do about it. The only thing that I can try at this point of time is to do a ITCH parser on 50 levels and use that for my seed value (which hopefully works).

09-05-2025. There is no ITCH 50 level that's available for 2012-06-21. Changing it so that I won't match completely with reference orderbook.

09-05-2025. Leaving this as is, as my engine works completely fine, it's just LOBSTER doesn't have data past it's price range. Next plan is to work making a ITCH binary into a complete (if possible lvl 50) message orderbook. 