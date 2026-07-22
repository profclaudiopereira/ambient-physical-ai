@echo off
cd /d "%~dp0.."

echo ============================================
echo Ambient Physical AI
echo AX630C Cognitive Runtime Deploy
echo ============================================
echo.

set HOST=root@192.168.77.15
set REMOTE=/root/ambient-runtime/runtime/cognitive/stackflow

echo Deploying Context Builder...
scp runtime\cognitive\stackflow\context_builder.py %HOST%:%REMOTE%/

echo Deploying Context Registry...
scp runtime\cognitive\stackflow\context_registry.py %HOST%:%REMOTE%/

echo Deploying Semantic Event Contract...
scp runtime\cognitive\stackflow\semantic_event.py %HOST%:%REMOTE%/

echo Deploying Semantic Event Generator...
scp runtime\cognitive\stackflow\semantic_event_generator.py %HOST%:%REMOTE%/

echo Deploying Semantic Dispatcher...
scp runtime\cognitive\stackflow\semantic_dispatcher.py %HOST%:%REMOTE%/

echo Deploying Identity UDP Listener...
scp runtime\cognitive\stackflow\identity_udp_listener.py %HOST%:%REMOTE%/

echo Deploying Semantic Services...
scp runtime\cognitive\stackflow\semantic_services.py %HOST%:%REMOTE%/

echo Deploying Semantic MCP Tools...
scp runtime\cognitive\stackflow\semantic_mcp_tools.py %HOST%:%REMOTE%/

echo Deploying StackChan MCP Server...
scp runtime\cognitive\stackflow\stackchan_mcp_server.py %HOST%:%REMOTE%/

echo Deploying StackChan Notifier...
scp runtime\cognitive\stackflow\stackchan_notifier.py %HOST%:%REMOTE%/

echo Deploying RGB Strip Notifier...
scp runtime\cognitive\stackflow\rgb_strip_notifier.py %HOST%:%REMOTE%/

echo.
echo ============================================
echo Deploy Finished
echo ============================================

pause