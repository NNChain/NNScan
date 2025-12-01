# NNScan Explorer - Endpoint Audit Report

**Date:** 2025-11-15  
**Purpose:** Document all endpoints required by each page for client reference

---

## Executive Summary

This document lists all the endpoints (data sources) that the NNScan Explorer application needs to function properly. Each page in the explorer requires specific endpoints to display data.

### Quick Overview:

- **Total Pages:** 17 pages
- **Main Data Source:** GraphQL/WebSocket (currently external - needs your own instance)
- **Working Endpoints:** RPC (26657) and REST (1317) are working
- **Critical Issue:** GraphQL endpoint is external and must be replaced with your own Hasura instance

### What You Need to Know:

1. **GraphQL/WebSocket** - This is the PRIMARY data source for most pages. Currently pointing to an external service that needs to be replaced with your own Hasura instance connected to your NNChain database.

2. **RPC Endpoint** (Port 26657) - ✅ Working - Used for blockchain queries, transaction proofs, and block data

3. **REST Endpoint** (Port 1317) - ✅ Working - Used for Cosmos SDK queries like supply, validators, accounts

4. **Oracle Endpoints** - ❌ Not implemented - Price data currently uses external APIs (CoinGecko) as fallback

5. **gRPC-Web** - ⚠️ Needs verification - Currently configured but may need separate port/path

---

## Current Configuration

| Environment Variable | Current Value | Type | Status |
|---------------------|---------------|------|--------|
| RPC_URL | http://155.133.26.60:26657/ | RPC (Tendermint) | ✅ Configured |
| REST_URL | http://155.133.26.60:1317/ | REST (Cosmos SDK) | ✅ Configured |
| GRAPHQL_URL | wss://devnet.d3n.xyz/hasura/v1/graphql | WebSocket (GraphQL) | ⚠️ External |
| GRPC | http://155.133.26.60:26657/ | gRPC-Web | ⚠️ Needs verification |
| LAMBDA_URL | (External) | HTTP | ⚠️ External |
| FAUCET_URL | http://155.133.26.60:1317/faucet/request | REST | ⚠️ Needs testing |

---

## Pages and Required Endpoints

### 1. Home Page (`/`)

**Components:**
- ChainInfoHighlights (NNC Price, Market Cap, Latest Block, Active Validators)
- TotalRequestsGraph
- LatestRequests
- LatestBlocks
- LatestTxTable

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Blocks, Requests, Transactions |
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Validators count, Bonded tokens |
| REST API | REST | `http://155.133.26.60:1317/cosmos/bank/v1beta1/supply` | ✅ Working | Token supply |
| REST API | REST | `http://155.133.26.60:1317/oracle/v1/request_prices` | ❌ Not Implemented | Price data (fallback to external APIs) |
| External API | HTTP | `https://api.coingecko.com/api/v3/...` | ✅ Working | Price data fallback |

---

### 2. Validators Page (`/validators`)

**Components:**
- ValidatorsTable
- Validator stats (Active, Bonded, Inflation, Block Time)

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Validators list, counts |
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Validator votes, blocks |

---

### 3. Blocks Page (`/blocks`)

**Components:**
- BlocksTable

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Blocks list |

---

### 4. Block Detail Page (`/block/{height}`)

**Components:**
- Block details
- Transactions in block

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Block by height |
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Transactions in block |

---

### 5. Transactions Page (`/txs`)

**Components:**
- TransactionsTable

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Transactions list |

---

### 6. Transaction Detail Page (`/tx/{hash}`)

**Components:**
- Transaction details
- Messages
- Proofs

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Transaction by hash |
| RPC API | RPC | `http://155.133.26.60:26657/tx?hash={hash}` | ✅ Available | Transaction proof |
| REST API | REST | `http://155.133.26.60:1317/cosmos/tx/v1beta1/txs/{hash}` | ⚠️ Needs testing | Transaction details |

---

### 7. Requests Page (`/requests`)

**Components:**
- RequestsTable

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Requests list |

---

### 8. Request Detail Page (`/request/{id}`)

**Components:**
- Request details
- Reports
- Proofs

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Request by ID |
| RPC API | RPC | `http://155.133.26.60:26657/abci_query?path=...` | ✅ Available | Request proof |

---

### 9. Account Page (`/account/{address}`)

**Components:**
- Account balance
- Delegations
- Unbonding
- Redelegations

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Account data |
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Delegations, Unbonding |
| REST API | REST | `http://155.133.26.60:1317/cosmos/auth/v1beta1/accounts/{address}` | ⚠️ Needs testing | Account info |
| REST API | REST | `http://155.133.26.60:1317/cosmos/bank/v1beta1/balances/{address}` | ⚠️ Needs testing | Account balance |

---

### 10. Validator Detail Page (`/validator/{address}`)

**Components:**
- Validator details
- Delegators
- Reports
- Proposed blocks

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Validator data |
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Delegators, Reports |
| REST API | REST | `http://155.133.26.60:1317/cosmos/staking/v1beta1/validators/{address}` | ⚠️ Needs testing | Validator info |

---

### 11. Proposals Page (`/proposals`)

**Components:**
- ProposalsTable

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Proposals list |

---

### 12. Proposal Detail Page (`/proposal/{id}`)

**Components:**
- Proposal details
- Votes

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Proposal by ID |
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Votes |

---

### 13. Data Sources Page (`/data-sources`)

**Components:**
- DataSourcesTable

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Data sources list |

---

### 14. Data Source Detail Page (`/data-source/{id}`)

**Components:**
- Data source details
- Code
- Execute
- Requests

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Data source by ID |
| LAMBDA_URL | HTTP | External function | ⚠️ External | Execute data source |

---

### 15. Oracle Scripts Page (`/oracle-scripts`)

**Components:**
- OracleScriptsTable

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Oracle scripts list |

---

### 16. Oracle Script Detail Page (`/oracle-script/{id}`)

**Components:**
- Oracle script details
- Code
- Bridge code
- Execute
- Requests

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | Oracle script by ID |
| gRPC | gRPC-Web | `http://155.133.26.60:26657/` | ⚠️ Needs verification | Send requests |
| LAMBDA_URL | HTTP | External function | ⚠️ External | Execute oracle script |

---

### 17. IBC Page (`/ibcs`)

**Components:**
- IBC connections
- Channels
- Packets

**Endpoints Required:**

| Endpoint | Type | URL/Path | Status | Notes |
|----------|------|----------|--------|-------|
| GraphQL Subscription | WebSocket | `wss://devnet.d3n.xyz/hasura/v1/graphql` | ⚠️ External | IBC data |

---

## Endpoint Type Summary

### 1. GraphQL/WebSocket (Hasura)
- **URL:** `wss://devnet.d3n.xyz/hasura/v1/graphql`
- **Type:** WebSocket (GraphQL Subscriptions)
- **Status:** ⚠️ **EXTERNAL - Needs to be replaced with your own Hasura instance**
- **Used by:** All pages for real-time data (blocks, transactions, validators, etc.)
- **Note:** This is the main data source. You'll need to set up your own Hasura instance connected to your NNChain database.

### 2. RPC (Tendermint)
- **URL:** `http://155.133.26.60:26657/`
- **Type:** RPC (JSON-RPC)
- **Status:** ✅ **WORKING**
- **Available Endpoints:**
  - `/status` - Node status
  - `/block?height={height}` - Block by height
  - `/tx?hash={hash}` - Transaction by hash
  - `/abci_query?path={path}` - Query application state
  - `/validators?height={height}` - Validators at height
  - And many more (see RPC documentation)

### 3. REST (Cosmos SDK)
- **URL:** `http://155.133.26.60:1317/`
- **Type:** REST API
- **Status:** ✅ **WORKING**
- **Available Endpoints:**
  - `/cosmos/bank/v1beta1/supply` - Token supply ✅ Tested
  - `/cosmos/staking/v1beta1/validators` - Validators ✅ Tested
  - `/cosmos/auth/v1beta1/accounts/{address}` - Account info
  - `/cosmos/bank/v1beta1/balances/{address}` - Account balance
  - `/cosmos/tx/v1beta1/txs/{hash}` - Transaction details
  - `/oracle/v1/request_prices` - ❌ Not Implemented

### 4. gRPC-Web
- **URL:** `http://155.133.26.60:26657/` (currently same as RPC)
- **Type:** gRPC-Web
- **Status:** ⚠️ **NEEDS VERIFICATION**
- **Note:** Typically gRPC-Web runs on a different port or path. May need to be configured separately.

### 5. External APIs
- **CoinGecko:** `https://api.coingecko.com/api/v3/...` - Price data fallback
- **CryptoCompare:** `https://min-api.cryptocompare.com/...` - Price data fallback
- **LAMBDA_URL:** External function for executing data sources/oracle scripts

---

## Critical Findings

### ⚠️ High Priority Issues:

1. **GraphQL/WebSocket Endpoint is External**
   - Currently using: `wss://devnet.d3n.xyz/hasura/v1/graphql`
   - **Action Required:** Set up your own Hasura instance connected to NNChain database
   - **Impact:** All pages depend on this for real-time data

2. **Oracle Endpoints Not Implemented**
   - `/oracle/v1/request_prices` returns "Not Implemented"
   - **Action Required:** Implement oracle endpoints or use external price APIs
   - **Impact:** Price data currently uses external APIs as fallback

3. **gRPC Endpoint Configuration**
   - Currently pointing to RPC endpoint
   - **Action Required:** Verify correct gRPC-Web endpoint URL
   - **Impact:** Transaction submission may not work correctly

### ✅ Working Endpoints:

1. **RPC Endpoint** - Fully functional, all standard Tendermint RPC endpoints available
2. **REST Endpoint** - Working for basic queries (supply, validators tested)
3. **External Price APIs** - Working as fallback for price data

---

## Recommendations

1. **Set up Hasura GraphQL instance** connected to your NNChain database
2. **Test all REST endpoints** to ensure they work with your chain
3. **Configure gRPC-Web endpoint** separately if needed
4. **Implement oracle endpoints** or continue using external price APIs
5. **Update environment variables** once all endpoints are confirmed

---

## Testing Results

### Tested and Working:
- ✅ RPC: `http://155.133.26.60:26657/status` - Node status
- ✅ RPC: `http://155.133.26.60:26657/block?height={height}` - Block data
- ✅ RPC: `http://155.133.26.60:26657/abci_query?path={path}` - Application queries
- ✅ RPC: `http://155.133.26.60:26657/tx_search?query={query}` - Transaction search
- ✅ RPC: `http://155.133.26.60:26657/validators?height={height}` - Validators list
- ✅ REST: `http://155.133.26.60:1317/cosmos/bank/v1beta1/supply` - Token supply
- ✅ REST: `http://155.133.26.60:1317/cosmos/staking/v1beta1/validators` - Validators
- ✅ REST: `http://155.133.26.60:1317/cosmos/staking/v1beta1/validators/{address}` - Validator details
- ✅ REST: `http://155.133.26.60:1317/cosmos/gov/v1beta1/proposals` - Proposals list

### Tested and Not Working:
- ❌ REST: `http://155.133.26.60:1317/oracle/v1/request_prices` - Returns "Not Implemented"
- ❌ REST: `http://155.133.26.60:1317/supply/circulating` - Returns "Not Implemented"
- ❌ REST: `http://155.133.26.60:1317/cosmos/tx/v1beta1/txs/{hash}` - Returns "tx not found" (needs valid hash)

### Needs Testing with Valid Data:
- ⚠️ REST: `/cosmos/auth/v1beta1/accounts/{address}` - Needs valid NNChain address
- ⚠️ REST: `/cosmos/bank/v1beta1/balances/{address}` - Needs valid NNChain address
- ⚠️ REST: `/cosmos/gov/v1beta1/proposals/{id}` - Returns "doesn't exist" (no proposals yet)
- ⚠️ gRPC-Web endpoint configuration - Currently same as RPC, may need separate port/path
