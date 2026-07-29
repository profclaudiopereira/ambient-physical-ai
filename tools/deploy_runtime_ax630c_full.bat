@echo off
setlocal EnableExtensions

rem ============================================================================
rem Ambient Physical AI - AX630C Cognitive Runtime Full Deploy
rem
rem This script copies the complete local StackFlow runtime tree to the AX630C,
rem including the services/ambient_context module required by the automatic
rem NFC -> Ambient Context -> Tab5 integration.
rem
rem The remote directory is created before deployment. Python cache artifacts
rem are removed from the AX630C after copying because they are machine-generated
rem and must not be treated as runtime source files.
rem ============================================================================

cd /d "%~dp0.."
if errorlevel 1 goto :local_error

set "HOST=root@192.168.77.15"
set "LOCAL=runtime\cognitive\stackflow"
set "REMOTE=/root/ambient-runtime/runtime/cognitive/stackflow"

echo ============================================
echo Ambient Physical AI
echo AX630C Cognitive Runtime Full Deploy
echo ============================================
echo.
echo Local directory : %LOCAL%
echo Target          : %HOST%
echo Remote directory: %REMOTE%
echo.

if not exist "%LOCAL%\identity_udp_listener.py" (
    echo ERROR: StackFlow runtime was not found at:
    echo        %CD%\%LOCAL%
    goto :local_error
)

echo [1/5] Checking SSH connection...
ssh "%HOST%" "echo AX630C connection: PASS"
if errorlevel 1 goto :deploy_error

echo.
echo [2/5] Creating the remote runtime directory...
ssh "%HOST%" "mkdir -p '%REMOTE%'"
if errorlevel 1 goto :deploy_error

echo.
echo [3/5] Deploying the complete StackFlow runtime tree...
echo       This includes root modules, services, labs, notes and documentation.
scp -r "%LOCAL%\." "%HOST%:%REMOTE%/"
if errorlevel 1 goto :deploy_error

echo.
echo [4/5] Removing generated Python cache files on the AX630C...
ssh "%HOST%" "find '%REMOTE%' -type d -name '__pycache__' -prune -exec rm -rf {} + 2>/dev/null; find '%REMOTE%' -type f -name '*.pyc' -delete 2>/dev/null"
if errorlevel 1 goto :deploy_error

echo.
echo [5/5] Validating deployed Python sources...
ssh "%HOST%" "cd '%REMOTE%' && python3 -m py_compile identity_udp_listener.py context_builder.py context_registry.py semantic_dispatcher.py semantic_event.py semantic_event_generator.py ambient_runtime_notifier.py rgb_strip_notifier.py runtime_state_notifier.py echo_pyramid_adapter.py services/ambient_context/ambient_context_service.py services/ambient_context/send_test_context.py"
if errorlevel 1 goto :validation_error

echo.
echo ============================================
echo Deploy Finished Successfully
echo ============================================
echo.
echo Complete StackFlow tree copied to:
echo   %HOST%:%REMOTE%
echo.
echo Ambient Context files deployed:
echo   - services/ambient_context/ambient_context_service.py
echo   - services/ambient_context/send_test_context.py
echo   - services/ambient_context/.env.example
echo   - services/ambient_context/ambient-context.service
echo.
echo Remote Python syntax validation: PASS
echo.
goto :end

:validation_error
echo.
echo ============================================
echo DEPLOY COMPLETED, VALIDATION FAILED
echo ============================================
echo Files were copied, but one or more deployed Python files did not compile.
echo Review the Python error shown above before restarting the runtime.
echo.
exit /b 2

:deploy_error
echo.
echo ============================================
echo DEPLOY FAILED
echo ============================================
echo The complete StackFlow tree was not deployed successfully.
echo Review the SSH or SCP error shown above.
echo.
exit /b 1

:local_error
echo.
echo ============================================
echo LOCAL DEPLOY ERROR
echo ============================================
echo Confirm that this BAT file is stored inside the repository tools folder
echo and that the repository contains runtime\cognitive\stackflow.
echo.
exit /b 3

:end
pause
endlocal
