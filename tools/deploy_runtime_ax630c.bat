@echo off
setlocal

cd /d "%~dp0.."

echo ============================================
echo Ambient Physical AI
echo AX630C Cognitive Runtime Deploy
echo ============================================
echo.

set "HOST=root@192.168.77.15"
set "REMOTE=/root/ambient-runtime/runtime/cognitive/stackflow"

echo Target: %HOST%
echo Remote directory: %REMOTE%
echo.

echo Deploying Context Builder...
scp runtime\cognitive\stackflow\context_builder.py %HOST%:%REMOTE%/
if errorlevel 1 goto :deploy_error

echo Deploying Context Registry...
scp runtime\cognitive\stackflow\context_registry.py %HOST%:%REMOTE%/
if errorlevel 1 goto :deploy_error

echo Deploying Semantic Event Contract...
scp runtime\cognitive\stackflow\semantic_event.py %HOST%:%REMOTE%/
if errorlevel 1 goto :deploy_error

echo Deploying Semantic Event Generator...
scp runtime\cognitive\stackflow\semantic_event_generator.py %HOST%:%REMOTE%/
if errorlevel 1 goto :deploy_error

echo Deploying Semantic Dispatcher...
scp runtime\cognitive\stackflow\semantic_dispatcher.py %HOST%:%REMOTE%/
if errorlevel 1 goto :deploy_error

echo Deploying Identity UDP Listener...
scp runtime\cognitive\stackflow\identity_udp_listener.py %HOST%:%REMOTE%/
if errorlevel 1 goto :deploy_error

echo Deploying Runtime State Notifier...
scp runtime\cognitive\stackflow\runtime_state_notifier.py %HOST%:%REMOTE%/
if errorlevel 1 goto :deploy_error

echo Deploying Semantic Services...
scp runtime\cognitive\stackflow\semantic_services.py %HOST%:%REMOTE%/
if errorlevel 1 goto :deploy_error

echo Deploying Semantic MCP Tools...
scp runtime\cognitive\stackflow\semantic_mcp_tools.py %HOST%:%REMOTE%/
if errorlevel 1 goto :deploy_error

echo Deploying StackChan MCP Server...
scp runtime\cognitive\stackflow\stackchan_mcp_server.py %HOST%:%REMOTE%/
if errorlevel 1 goto :deploy_error

echo Deploying StackChan Notifier...
scp runtime\cognitive\stackflow\stackchan_notifier.py %HOST%:%REMOTE%/
if errorlevel 1 goto :deploy_error

echo Deploying RGB Expression Notifier...
scp runtime\cognitive\stackflow\rgb_strip_notifier.py %HOST%:%REMOTE%/
if errorlevel 1 goto :deploy_error

echo Deploying Ambient Runtime Notifier...
scp runtime\cognitive\stackflow\ambient_runtime_notifier.py %HOST%:%REMOTE%/
if errorlevel 1 goto :deploy_error

echo Deploying Echo Pyramid Adapter...
scp runtime\cognitive\stackflow\echo_pyramid_adapter.py %HOST%:%REMOTE%/
if errorlevel 1 goto :deploy_error

echo.
echo ============================================
echo Deploy Finished Successfully
echo ============================================
echo.
echo Updated integration files include:
echo   - identity_udp_listener.py
echo   - runtime_state_notifier.py
echo   - rgb_strip_notifier.py
echo   - echo_pyramid_adapter.py
echo.
goto :end

:deploy_error
echo.
echo ============================================
echo DEPLOY FAILED
echo ============================================
echo One or more files were not copied to the AX630C.
echo Review the SCP error shown above before starting the runtime.
echo.
exit /b 1

:end
pause
endlocal
