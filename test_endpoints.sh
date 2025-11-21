#!/bin/bash
echo "=== Testing NNChain Endpoints ==="
echo ""

echo "1. RPC Endpoint - Status:"
curl -s "http://155.133.26.60:26657/status" | python3 -c "import sys, json; d=json.load(sys.stdin); print('✅ Working' if 'result' in d else '❌ Failed')" 2>/dev/null || echo "❌ Failed"
echo ""

echo "2. REST Endpoint - Supply:"
curl -s "http://155.133.26.60:1317/cosmos/bank/v1beta1/supply" | python3 -c "import sys, json; d=json.load(sys.stdin); print('✅ Working' if 'supply' in d else '❌ Failed')" 2>/dev/null || echo "❌ Failed"
echo ""

echo "3. REST Endpoint - Validators:"
curl -s "http://155.133.26.60:1317/cosmos/staking/v1beta1/validators" | python3 -c "import sys, json; d=json.load(sys.stdin); print('✅ Working' if 'validators' in d else '❌ Failed')" 2>/dev/null || echo "❌ Failed"
echo ""

echo "4. REST Endpoint - Node Info:"
curl -s "http://155.133.26.60:1317/cosmos/base/tendermint/v1beta1/node_info" | python3 -c "import sys, json; d=json.load(sys.stdin); print('✅ Working' if 'default_node_info' in d else '❌ Failed')" 2>/dev/null || echo "❌ Failed"
echo ""

echo "5. REST Endpoint - Oracle (Expected to fail):"
curl -s "http://155.133.26.60:1317/oracle/v1/request_prices" | python3 -c "import sys, json; d=json.load(sys.stdin); print('❌ Not Implemented' if d.get('code') == 12 else '✅ Working')" 2>/dev/null || echo "❌ Failed"
echo ""

echo "6. REST Endpoint - Faucet:"
curl -s -X POST "http://155.133.26.60:1317/faucet/request" -H "Content-Type: application/json" -d '{}' | python3 -c "import sys, json; d=json.load(sys.stdin); print('❌ Not Implemented' if d.get('code') == 12 else '✅ Working')" 2>/dev/null || echo "❌ Failed"
echo ""

echo "7. GraphQL Endpoint (WebSocket - cannot test with curl):"
echo "⚠️  Needs manual testing - Current: wss://devnet.d3n.xyz/hasura/v1/graphql"
echo ""

echo "8. gRPC Endpoint:"
curl -s "http://155.133.26.60:9090/" 2>&1 | head -1 || echo "⚠️  gRPC typically not accessible via HTTP curl"
