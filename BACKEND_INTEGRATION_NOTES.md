# Backend Integration Notes

## Status: Phase 1 Complete, Phase 2 In Progress

### Phase 1: Setup ✅
- ✅ Environment variables updated (API_BASE_URL, WS_URL)
- ✅ Backend API client created (`src/api/BackendApi.re`)
- ✅ WebSocket client created (`src/api/WebSocketClient.re`)
- ✅ Backend hooks created (`src/api/BackendHooks.re`)

### Phase 2: Page Integration (In Progress)

#### Home Page Integration
- ✅ `BlockSubBackend.re` created - uses REST API + WebSocket for blocks
- ⚠️ `RequestSubBackend.re` - needs to be created
- ⚠️ `TxSubBackend.re` - needs to be created

### Important Notes

1. **API Response Format**: The parsers in backend subscription modules make assumptions about the API response format. They may need adjustment based on actual API responses.

2. **Testing Required**: Each page should be tested in the browser after integration to verify:
   - Data loads correctly
   - UI components render properly
   - Real-time updates work via WebSocket
   - Error handling works

3. **Parser Adjustments**: The parsers handle:
   - Standardized response format: `{ success: true, data: {...} }`
   - Paginated responses: `{ success: true, data: [...], pagination: {...} }`
   - Direct data objects (if no wrapper)

4. **WebSocket Integration**: WebSocket subscriptions are set up for:
   - `blocks` channel - real-time block updates
   - `transactions` channel - real-time transaction updates
   - `validators` channel - real-time validator updates

### Next Steps

1. Complete RequestSubBackend and TxSubBackend
2. Update HomePage to use backend modules
3. Test HomePage in browser
4. Adjust parsers based on actual API responses
5. Continue with other pages

