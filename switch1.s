

Read raw bits
Compare with last value and set changed or not
Compute pending flag (if changed and not pending, then pending)

Old pending     Changed     New pending
    0             0           0
    0             1           1
    1             0           1
    1             1           1

During normal switch processing, scan for pending switches
When queued, turn on the pending bits


switch_rtt:
   lda      switch_raw_bits
   sta      switch_prev_bits
   lda      IO
   sta      switch_raw_bits
   eora     switch_prev_bits
   sta      switch_changed_bits
   ora      switch_pending_bits
   sta      switch_pending_bits



