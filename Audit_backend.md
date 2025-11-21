# Professional Audit Report
## Directory Structure Analysis

**Date:** January 2025  
**Auditor:** Technical Analysis Team  
**Scope:** Root Directory Audit - Four Primary Directories

---

## Executive Summary

This audit examines four directories in the root filesystem: `backend`, `go`, `NN-Chain`, and `terra-docker`. The analysis reveals a blockchain infrastructure project built on the Cosmos SDK framework, specifically implementing a custom blockchain called "NN-Chain" (or "NN-Terra") with NFT (Non-Fungible Token) functionality. The system includes a Node.js backend API, Go-based blockchain infrastructure, and Docker containerization for deployment.

**Key Findings:**
- Custom Cosmos SDK-based blockchain implementation
- NFT management system with WASM smart contract integration
- RESTful API backend for blockchain interactions
- Docker-based deployment infrastructure
- Production-ready blockchain node setup

---

## 1. Directory: `/root/backend`

### 1.1 Overview
A Node.js/Express.js REST API backend that serves as an interface layer between client applications and a Cosmos-based blockchain network. The backend specifically handles NFT (Non-Fungible Token) operations through WASM (WebAssembly) smart contracts.

### 1.2 Technology Stack
- **Runtime:** Node.js
- **Framework:** Express.js v5.1.0
- **Key Dependencies:**
  - `express`: Web server framework
  - `body-parser`: Request body parsing
  - `dotenv`: Environment variable management
  - `node-fetch`: HTTP client for blockchain node communication
  - `pm2`: Process manager for production deployment
  - `child_process`: For executing blockchain CLI commands

### 1.3 Architecture
The backend follows a standard MVC-like pattern:
- **Entry Point:** `app.js` - Server initialization and middleware setup
- **Routes:** `routes.js` - API endpoint definitions
- **Controller:** `controller.js` - Business logic and blockchain interactions

### 1.4 API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/nfts` | GET | Retrieve all NFTs from the blockchain |
| `/api/nft/:id` | GET | Get specific NFT details by token ID |
| `/api/mint` | POST | Mint a new NFT (owner-only operation) |
| `/api/transfer` | POST | Transfer NFT ownership (owner-only operation) |
| `/api/owner/:id` | GET | Get owner address for a specific token ID |
| `/api/:address` | GET | Debug endpoint to decode blockchain addresses |

### 1.5 Blockchain Integration

**Network Configuration:**
- **Chain ID:** `NNTERRA-1` (default, configurable via environment)
- **RPC Node:** `http://155.133.26.60:1317` (REST API endpoint)
- **RPC Node (Tendermint):** `tcp://155.133.26.60:26657` (Tendermint RPC)
- **Home Directory:** `~/.simd` (blockchain node data directory)
- **Keyring:** File-based keyring backend
- **Gas Configuration:** 500,000 units at 0.1uNC per unit

**Smart Contract Interaction:**
- Uses CosmWasm (Cosmos WebAssembly) for smart contract execution
- Communicates with WASM contracts deployed on the blockchain
- Executes transactions via `wasmd` CLI commands
- Queries contract state via REST API calls

### 1.6 Security Features
- Owner-only minting and transfer operations
- Environment variable-based configuration
- Keyring passphrase protection (optional)
- Input validation for required parameters

### 1.7 Environment Variables Required
```bash
CONTRACT          # Smart contract address
NFT_OWNER         # Default NFT owner address
WASMD_NODE        # REST API endpoint
WASMD_RPC_NODE    # Tendermint RPC endpoint
WASMD_HOME        # Blockchain node home directory
WASMD_KEY         # Keyring key name (default: 'validator')
WASMD_PASS        # Keyring passphrase (optional)
CHAIN_ID          # Blockchain chain ID (default: 'NNTERRA-1')
GAS               # Gas limit (default: '500000')
GAS_PRICES        # Gas price (default: '0.1uNC')
PORT              # Server port (default: 3000)
```

### 1.8 Code Quality Observations
- ✅ Clean separation of concerns (routes, controller, app)
- ✅ Error handling implemented
- ✅ Async/await pattern used correctly
- ⚠️ No authentication middleware visible
- ⚠️ No rate limiting implemented
- ⚠️ No request logging/monitoring visible
- ⚠️ Hardcoded default node addresses in code

### 1.9 Deployment
- Uses PM2 for process management
- Default port: 3000
- Requires access to blockchain node
- Requires `.env` file for configuration

---

## 2. Directory: `/root/go`

### 2.1 Overview
This directory contains the Go module cache and a compiled binary. It serves as the Go workspace and dependency storage for Go-based blockchain projects.

### 2.2 Contents

**Structure:**
```
/go
├── bin/
│   └── simd          # Compiled Cosmos SDK simulator daemon binary
└── pkg/
    └── mod/          # Go module cache (downloaded dependencies)
```

### 2.3 Binary Analysis: `simd`

**File:** `/root/go/bin/simd`
- **Type:** ELF 64-bit LSB executable (Linux x86-64)
- **Purpose:** Cosmos SDK Simulator Daemon
- **Description:** This is the compiled blockchain node binary built from the Cosmos SDK. The `simd` (simulator daemon) is a reference implementation used for testing and development, but can also be used as a base for custom blockchains.

**Key Capabilities:**
- Blockchain node operation
- Transaction processing
- State management
- Consensus participation
- RPC and REST API serving

### 2.4 Go Module Cache
The `pkg/mod/` directory contains all downloaded Go module dependencies, including:
- **Cosmos SDK modules:** Core blockchain framework components
- **CometBFT:** Consensus engine (formerly Tendermint)
- **CosmWasm:** WebAssembly smart contract support
- **Google Protobuf:** Protocol buffer libraries
- **Various utilities:** Crypto, logging, networking libraries

**Notable Dependencies:**
- `cosmossdk.io/*` - Cosmos SDK core modules
- `github.com/cometbft/cometbft` - Consensus engine
- `github.com/cosmos/cosmos-sdk` - Main SDK framework
- `google.golang.org/grpc` - gRPC communication
- `github.com/CosmWasm/wasmd` - WASM smart contract support

### 2.5 Purpose in Project
- Provides compiled blockchain node binary
- Stores Go dependencies for blockchain development
- Supports building and running custom blockchain applications
- Contains all necessary libraries for Cosmos SDK development

---

## 3. Directory: `/root/NN-Chain`

### 3.1 Overview
This is a **complete Cosmos SDK framework repository**, likely a fork or customized version of the official Cosmos SDK. The name "NN-Chain" suggests this is the source code for a custom blockchain implementation, possibly named "NN-Chain" or "NN-Terra" (based on the chain ID `NNTERRA-1` found in the backend).

### 3.2 Technology Stack
- **Language:** Go 1.23.2
- **Framework:** Cosmos SDK (latest version, appears to be v0.53.x based on CHANGELOG)
- **Consensus:** CometBFT v0.38.17
- **Smart Contracts:** CosmWasm support (WASM-based)
- **Build System:** Make

### 3.3 Repository Structure

**Core Components:**

| Directory | Purpose |
|-----------|---------|
| `api/` | Protocol buffer API definitions |
| `baseapp/` | Base application framework for blockchain apps |
| `client/` | CLI client and query tools |
| `codec/` | Encoding/decoding (Amino, Protobuf, JSON) |
| `collections/` | State management collections |
| `core/` | Core blockchain primitives (addresses, coins, events, etc.) |
| `crypto/` | Cryptographic functions and key management |
| `depinject/` | Dependency injection framework |
| `errors/` | Error handling utilities |
| `log/` | Logging framework |
| `math/` | Mathematical utilities |
| `proto/` | Protocol buffer definitions |
| `server/` | Server components (RPC, REST, gRPC) |
| `simapp/` | Simulator application (reference implementation) |
| `store/` | State storage (IAVL trees, etc.) |
| `types/` | Core data types |
| `x/` | Cosmos SDK modules (banking, staking, governance, etc.) |

### 3.4 Key Features

**Blockchain Capabilities:**
- ✅ Full Cosmos SDK implementation
- ✅ CometBFT consensus integration
- ✅ WASM smart contract support (CosmWasm)
- ✅ IBC (Inter-Blockchain Communication) ready
- ✅ Modular architecture
- ✅ gRPC and REST API support
- ✅ CLI tools for node management
- ✅ Key management and wallet support

**Development Tools:**
- Docker support (`Dockerfile`, `docker-compose.yml`)
- Makefile for building
- Testing infrastructure
- Documentation (`docs/`)
- Code generation tools

### 3.5 Build Configuration

**Go Module:** `github.com/cosmos/cosmos-sdk`
- **Go Version:** 1.23.2
- **Main Dependencies:**
  - CometBFT v0.38.17
  - CosmWasm integration
  - Protocol Buffers
  - gRPC

**Docker Configuration:**
- Multi-stage build process
- Alpine Linux base image
- Supports cross-compilation
- Exposes ports: 26656 (P2P), 26657 (RPC), 1317 (REST), 9090 (gRPC)

### 3.6 Docker Compose Setup
The repository includes a `docker-compose.yml` for local network testing:
- **4-node test network** configuration
- Each node on separate IP addresses (192.168.10.2-5)
- Separate ports for each node
- Shared volume for testnet data

### 3.7 Relationship to Project
This directory contains the **source code** for the blockchain itself. When compiled, it produces the `simd` binary found in `/root/go/bin/` and `/root/terra-docker/`. The backend API communicates with nodes running this blockchain software.

**Customization Points:**
- Chain ID: `NNTERRA-1` (suggests Terra-based or Terra-inspired)
- Custom modules may be added in `x/` directory
- Genesis configuration
- Consensus parameters
- Gas pricing

### 3.8 Documentation
- `README.md` - Main documentation
- `CHANGELOG.md` - Version history
- `CONTRIBUTING.md` - Contribution guidelines
- `UPGRADE_GUIDE.md` - Upgrade instructions
- `ROADMAP.md` - Development roadmap
- `SECURITY.md` - Security policies

---

## 4. Directory: `/root/terra-docker`

### 4.1 Overview
Docker containerization setup for deploying and running a Terra-based blockchain node. This directory provides a production-ready containerized environment for running the blockchain node.

### 4.2 Contents

**Files:**
- `Dockerfile` - Container image definition
- `docker-compose.yml` - Multi-container orchestration
- `entrypoint.sh` - Container startup script
- `simd` - Pre-compiled blockchain binary (same as in `/root/go/bin/`)

### 4.3 Dockerfile Analysis

**Base Image:** Ubuntu 24.04
**Purpose:** Runtime container for blockchain node

**Key Features:**
- Minimal dependencies (curl, jq)
- Pre-compiled `simd` binary copied to `/usr/bin/simd`
- Custom entrypoint script
- Configurable via environment variables

**Build Process:**
The Dockerfile is simplified - it uses a pre-compiled binary rather than building from source. This suggests:
- Binary is built separately (likely from NN-Chain source)
- Faster container startup
- Smaller image size
- Production deployment focus

### 4.4 Docker Compose Configuration

**Service:** `validator1`
- **Container Name:** `validator1`
- **Node Home:** `/root/.simapp` (mapped to host)
- **Ports Exposed:**
  - `26656` - P2P networking
  - `26657` - Tendermint RPC
  - `9090` - gRPC API
  - `1317` - REST API

**Volume Mapping:**
- Host: `/root/.simapp`
- Container: `/root/.simapp`
- Purpose: Persistent blockchain state storage

### 4.5 Entrypoint Script

**File:** `entrypoint.sh`
- Sets default node home directory
- Configurable node name
- Executes `simd start` command
- Passes command-line arguments

**Usage Pattern:**
```bash
simd start --home "$NODE_HOME" [additional-args]
```

### 4.6 Deployment Architecture

**Single Validator Setup:**
- One validator node
- Persistent state via volume mount
- All standard blockchain ports exposed
- Ready for network participation

**Configuration:**
- Environment variables for customization
- Command-line argument passthrough
- Flexible startup options

### 4.7 Relationship to Other Directories
- **Uses binary from:** `/root/go/bin/simd` (or built from `/root/NN-Chain`)
- **Serves API for:** `/root/backend` (REST API at port 1317)
- **Blockchain source:** `/root/NN-Chain` (if building from source)

---

## 5. System Architecture Overview

### 5.1 Component Relationships

```
┌─────────────────┐
│   Client Apps   │
└────────┬────────┘
         │ HTTP/REST
         ▼
┌─────────────────┐
│  Backend API    │  ← /root/backend
│  (Node.js)      │
│  Port: 3000     │
└────────┬────────┘
         │ HTTP/REST + CLI
         ▼
┌─────────────────┐
│  Blockchain     │  ← /root/terra-docker
│  Node (simd)    │     (Docker container)
│  Ports:         │
│  1317 (REST)    │
│  26657 (RPC)    │
│  26656 (P2P)    │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Blockchain     │  ← /root/NN-Chain
│  Network        │     (Source code)
│  (NNTERRA-1)    │
└─────────────────┘
```

### 5.2 Data Flow

1. **Client Request** → Backend API (`/root/backend`)
2. **Backend Processing** → Queries/Executes on Blockchain Node
3. **Blockchain Node** → Processes via Cosmos SDK (`/root/NN-Chain`)
4. **State Changes** → Stored in node's persistent storage
5. **Response** → Returned to client via Backend API

### 5.3 Technology Stack Summary

| Component | Technology | Purpose |
|-----------|-----------|---------|
| Backend API | Node.js + Express | REST API layer |
| Blockchain Node | Go + Cosmos SDK | Blockchain runtime |
| Consensus | CometBFT | Byzantine Fault Tolerance |
| Smart Contracts | CosmWasm (WASM) | NFT contract execution |
| Containerization | Docker | Deployment & orchestration |
| State Storage | IAVL Trees | Merkle tree state |

---

## 6. Key Findings & Recommendations

### 6.1 Strengths
✅ **Well-structured architecture** - Clear separation between API layer and blockchain  
✅ **Production-ready setup** - Docker containerization for deployment  
✅ **Modern stack** - Latest Cosmos SDK with WASM support  
✅ **Modular design** - Easy to extend and maintain  
✅ **Complete infrastructure** - From source code to deployment  

### 6.2 Areas for Improvement

**Security:**
- ⚠️ Backend API lacks authentication/authorization middleware
- ⚠️ No rate limiting on API endpoints
- ⚠️ Hardcoded default node addresses should be environment-only
- ⚠️ Keyring passphrase handling could be more secure
- ⚠️ No HTTPS/TLS configuration visible

**Monitoring & Observability:**
- ⚠️ No logging framework visible in backend
- ⚠️ No metrics/monitoring setup
- ⚠️ No health check endpoints
- ⚠️ No error tracking (e.g., Sentry)

**Code Quality:**
- ⚠️ Backend lacks unit tests
- ⚠️ No API documentation (Swagger/OpenAPI)
- ⚠️ Error messages could be more user-friendly
- ⚠️ No input sanitization visible

**Deployment:**
- ⚠️ No CI/CD pipeline visible
- ⚠️ No backup strategy documented
- ⚠️ No disaster recovery plan
- ⚠️ Single validator setup (consider multi-validator for production)

### 6.3 Critical Questions for Client Meeting

1. **Network Configuration:**
   - Is `NNTERRA-1` a testnet or mainnet?
   - What is the relationship to Terra blockchain?
   - Are there other validators in the network?

2. **Smart Contracts:**
   - Where is the NFT smart contract code?
   - Is the contract deployed and verified?
   - What are the contract's capabilities beyond mint/transfer?

3. **Security:**
   - What authentication mechanism should be implemented?
   - Who has access to the validator keys?
   - What is the key management strategy?

4. **Deployment:**
   - Is this a production or development environment?
   - What is the expected transaction volume?
   - What are the scaling requirements?

5. **Maintenance:**
   - Who maintains the blockchain node?
   - What is the upgrade strategy?
   - How are backups handled?

6. **Business Logic:**
   - What is the purpose of this NFT system?
   - Are there additional features planned?
   - What is the tokenomics model?

---

## 7. Technical Specifications

### 7.1 Network Configuration

**Chain ID:** `NNTERRA-1`  
**Default Node:** `155.133.26.60`  
**Ports:**
- P2P: 26656
- RPC: 26657
- REST: 1317
- gRPC: 9090

**Gas Configuration:**
- Default Gas: 500,000
- Gas Price: 0.1uNC

### 7.2 File Structure Summary

```
/root/
├── backend/              # Node.js API server
│   ├── app.js           # Server entry point
│   ├── routes.js        # API routes
│   ├── controller.js    # Business logic
│   └── package.json     # Dependencies
│
├── go/                  # Go workspace
│   ├── bin/simd         # Compiled blockchain binary
│   └── pkg/mod/         # Go module cache
│
├── NN-Chain/            # Cosmos SDK source code
│   ├── baseapp/         # Core application framework
│   ├── client/          # CLI tools
│   ├── x/               # SDK modules
│   ├── go.mod           # Go module definition
│   └── Dockerfile       # Build configuration
│
└── terra-docker/        # Docker deployment
    ├── Dockerfile       # Container definition
    ├── docker-compose.yml
    ├── entrypoint.sh    # Startup script
    └── simd             # Pre-compiled binary
```

### 7.3 Dependencies Summary

**Backend (Node.js):**
- express: ^5.1.0
- body-parser: ^2.2.0
- dotenv: ^17.2.3
- node-fetch: ^3.3.2
- pm2: ^6.0.13

**Blockchain (Go):**
- Cosmos SDK: Latest (v0.53.x)
- CometBFT: v0.38.17
- CosmWasm: Integrated
- Go: 1.23.2

---

## 8. Conclusion

This audit reveals a **complete blockchain infrastructure project** built on the Cosmos SDK framework. The system implements a custom blockchain (NN-Chain/NN-Terra) with NFT functionality, supported by a Node.js REST API backend and Docker-based deployment.

**Project Maturity:** The codebase appears to be in a **functional but early-stage** state, suitable for development and testing. For production deployment, several security and operational improvements are recommended.

**Next Steps:**
1. Review security requirements with client
2. Implement authentication/authorization
3. Add monitoring and logging
4. Document API endpoints
5. Establish backup and recovery procedures
6. Plan for multi-validator network if needed

---

## Appendix A: Quick Reference

### A.1 Starting the System

**Backend API:**
```bash
cd /root/backend
npm install
# Create .env file with required variables
npm start  # or use pm2
```

**Blockchain Node (Docker):**
```bash
cd /root/terra-docker
docker-compose up -d
```

**Blockchain Node (Direct):**
```bash
/root/go/bin/simd start --home ~/.simd
```

### A.2 Key Commands

**Query NFT:**
```bash
curl http://localhost:3000/api/nft/1
```

**Mint NFT:**
```bash
curl -X POST http://localhost:3000/api/mint \
  -H "Content-Type: application/json" \
  -d '{"token_id": "1", "token_uri": "https://example.com/nft/1"}'
```

**Transfer NFT:**
```bash
curl -X POST http://localhost:3000/api/transfer \
  -H "Content-Type: application/json" \
  -d '{"token_id": "1", "new_owner": "terra1..."}'
```

### A.3 Important Paths

- Backend: `/root/backend`
- Blockchain Source: `/root/NN-Chain`
- Blockchain Binary: `/root/go/bin/simd`
- Docker Setup: `/root/terra-docker`
- Node Data: `~/.simd` or `/root/.simapp`

---

**End of Audit Report**

*This report provides a comprehensive overview of the four directories. For specific implementation details, refer to the source code in each directory.*

