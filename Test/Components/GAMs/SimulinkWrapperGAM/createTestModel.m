function model_compiled = createTestModel(varargin)

evalin('base', 'clear matrixConstant vectorConstant structScalar structMixed');

global model_name  model_compiled

warning('off', 'all');

%% settings

% default values
modelComplexity      = 1;
hasAllocFcn          = true;
hasGetmmiFcn         = true;
hasTunableParams     = false;
hasStructParams      = false;
hasStructArrayParams = false;
hasInputs            = true;
hasOutputs           = true;
hasStructSignals     = false;
dataOrientation      = 'Column-major';   isRowMajor = 0;
useType              = 0;

while ~isempty(varargin)
    
    switch varargin{1}
        
        case 'modelComplexity'
            modelComplexity = varargin{2};
        
        case 'hasAllocFcn'
            hasAllocFcn = varargin{2};
        
        case 'hasGetmmiFcn'
            hasGetmmiFcn = varargin{2};
            
        case 'hasTunableParams'
            hasTunableParams = varargin{2};
        
        case 'hasStructParams'
            hasStructParams = varargin{2};
        
        case 'hasStructArrayParams'
            hasStructArrayParams = varargin{2};
        
        case 'hasInputs'
            hasInputs = varargin{2};
            
        case 'hasOutputs'
            hasOutputs = varargin{2};
            
        case 'hasStructSignals'
            hasStructSignals = varargin{2};
        
        case 'dataOrientation'
            dataOrientation = varargin{2};
            if strcmp(dataOrientation, 'Row-major')
                isRowMajor = 1;
            end
            
        case 'useType'
            useType = varargin{2};
            
        otherwise
            error(['Unexpected option: ' varargin{1}])
    end
    
    varargin(1:2) = [];
end

% warning: the model name is limited to 20 characters
model_name = ['testModel' int2str(modelComplexity)  int2str(hasAllocFcn)     int2str(hasGetmmiFcn) ...
                          int2str(hasTunableParams) int2str(hasStructParams) int2str(hasStructArrayParams) ...
                          int2str(hasInputs)        int2str(hasOutputs)      int2str(hasStructSignals) ...
                          int2str(useType)          int2str(isRowMajor) ...
             ];

model_compiled = false;

if isfile([model_name '.so'])
    return
end

%% define constants

if hasTunableParams == true
    evalin('base', 'matrixConstant = [1 1 1; 2 2 2; 3 3 3];');
    evalin('base', 'vectorConstant = ones(10,1);');
    
    if modelComplexity >= 2
        evalin('base', 'vectorConstant2 = ones(8,1);');
        if modelComplexity >= 3
            evalin('base', 'matrixConstant2 = ones(6,6);');
            if modelComplexity >= 4
                evalin('base', 'matrixConstant3d = ones(4,4,4);');
            end
        end
    end
end

if hasStructParams == true
    
    evalin('base', 'structScalar.one = 1;');
    evalin('base', 'structScalar.nested1.one = 1;');
    evalin('base', 'structScalar.nested1.two = 2;');
    evalin('base', 'structScalar.nested2.one = 1;');
    evalin('base', 'structScalar.nested2.two = 2;');
    
    if modelComplexity >= 2
        evalin('base', 'structMixed.one = 1;');
        evalin('base', 'structMixed.vec = ones(8, 1)*2;');
        if modelComplexity >= 3
            evalin('base', 'structMixed.mat = uint32(ones(6, 6)*2);');
            if modelComplexity >= 4
                evalin('base', 'structMixed.mat3d = uint32(ones(4, 4, 4)*2);');
            end
        end
    end
    
end

if hasStructArrayParams == true
    
    evalin('base', 'structParamArray(1).one = 1;');
    evalin('base', 'structParamArray(1).two = 2;');
    evalin('base', 'structParamArray(2).one = 10;');
    evalin('base', 'structParamArray(2).two = 20;');
    
    evalin('base',  'structMixed.structParamArray(1).one = 1;');
    evalin('base',  'structMixed.structParamArray(1).two = 1;');
    evalin('base',  'structMixed.structParamArray(2).one = 1;');
    evalin('base',  'structMixed.structParamArray(2).two = 1;');
    
end

%% creating a new model

% delete model if it already exists
warning('off','MATLAB:DELETE:FileNotFound');
delete(sprintf('%s.slx',model_name));

% create the new model
new_system(model_name);
save_system(model_name);

%% adding blocks

% -- root system

% math blocks
add_block('simulink/Math Operations/Gain', [model_name '/Gain1']);
add_block('simulink/Math Operations/Gain', [model_name '/Gain2']);

if modelComplexity >= 2
    add_block('simulink/Math Operations/Gain', [model_name '/Gain3'], ...
        'Gain',           '1', ...
        'OutDataTypeStr', 'Inherit: Same as input');
    add_block('simulink/Math Operations/Gain', [model_name '/Gain4'], ...
        'Gain',           '1', ...
        'OutDataTypeStr', 'Inherit: Same as input');
    if hasStructParams == true
        set_param([model_name '/Gain3'], 'Gain', 'structMixed.one');
    end
end

if modelComplexity >= 3
    add_block('simulink/Math Operations/Gain', [model_name '/Gain5'], ...
        'Gain',           '1', ...
        'OutDataTypeStr', 'Inherit: Same as input');
    add_block('simulink/Math Operations/Gain', [model_name '/Gain6'], ...
        'Gain',           '1', ...
        'OutDataTypeStr', 'Inherit: Same as input');
end

% input ports
if hasInputs == true
    add_block('simulink/Sources/In1', [model_name '/In1_ScalarDouble']);
    set_param([model_name '/In1_ScalarDouble'], 'IconDisplay',    'Signal name');
    set_param([model_name '/In1_ScalarDouble'], 'OutDataTypeStr', 'double');
    
    add_block('simulink/Sources/In1', [model_name '/In2_ScalarUint32' ]);
    set_param([model_name '/In2_ScalarUint32'],  'IconDisplay',    'Signal name');
    set_param([model_name '/In2_ScalarUint32'],  'OutDataTypeStr', 'uint32');
    
    if modelComplexity >= 2
        add_block('simulink/Sources/In1', [model_name '/In3_VectorDouble' ], ...
            'IconDisplay',    'Signal name', ...
            'OutdataTypeStr', 'double',      ...
            'PortDimensions', '[8 1]');

        add_block('simulink/Sources/In1', [model_name '/In4_VectorUint32' ], ...
            'IconDisplay',    'Signal name', ...
            'OutdataTypeStr', 'uint32',      ...
            'PortDimensions', '[8 1]' );
    end
    
    if modelComplexity >= 3
        add_block('simulink/Sources/In1', [model_name '/In5_MatrixDouble' ], ...
            'IconDisplay',    'Signal name', ...
            'OutdataTypeStr', 'double',      ...
            'PortDimensions', '[6 6]' );

        add_block('simulink/Sources/In1', [model_name '/In6_MatrixUint32' ], ...
            'IconDisplay',    'Signal name', ...
            'OutdataTypeStr', 'uint32',      ...
            'PortDimensions', '[6 6]' );
    end
    
    if modelComplexity >= 4
        add_block('simulink/Sources/In1', [model_name '/In7_3DMatrixDouble' ], ...
            'IconDisplay',    'Signal name', ...
            'OutdataTypeStr', 'double', ...
            'PortDimensions',  '[4 4]');

        add_block('simulink/Sources/In1', [model_name '/In8_3DMatrixUint32' ], ...
            'IconDisplay',    'Signal name', ...
            'OutdataTypeStr', 'uint32', ...
            'PortDimensions',  '[4 4]');
        
        add_block('simulink/Math Operations/Matrix Concatenate', [model_name '/Concatenate7'], ...
            'NumInputs',            '2', ...
            'ConcatenateDimension', '3')
        
        add_block('simulink/Math Operations/Matrix Concatenate', [model_name '/Concatenate8'], ...
            'NumInputs',            '2', ...
            'ConcatenateDimension', '3')
    end
    
else
    add_block('simulink/Sources/Constant', [model_name '/In1_ScalarDouble']);
    set_param([model_name '/In1_ScalarDouble'], 'Value',          '1');
    set_param([model_name '/In1_ScalarDouble'], 'OutDataTypeStr', 'double');
    
    add_block('simulink/Sources/Constant', [model_name '/In2_ScalarUint32']);
    set_param([model_name '/In2_ScalarUint32'], 'Value',          '1');
    set_param([model_name '/In2_ScalarUint32'],  'OutDataTypeStr', 'uint32');
    
    if (modelComplexity >= 2)
        add_block('simulink/Sources/Constant', [model_name '/In3_VectorDouble']);
        set_param([model_name '/In3_VectorDouble'], 'OutDataTypeStr', 'double');
        if hasTunableParams == true
            set_param([model_name '/In3_VectorDouble'],  'Value',          'vectorConstant2');
        else
            set_param([model_name '/In3_VectorDouble'],  'Value',          'rand(8,1)');
        end
        
        add_block('simulink/Sources/Constant', [model_name '/In4_VectorUint32']);
        set_param([model_name '/In4_VectorUint32'],  'OutDataTypeStr', 'uint32');
        if hasStructParams == true
            set_param([model_name '/In4_VectorUint32'], 'Value',          'structMixed.vec');
        else
            set_param([model_name '/In4_VectorUint32'], 'Value',          'ones(8,1)');
        end
    end
    
    if (modelComplexity >= 3)
        add_block('simulink/Sources/Constant', [model_name '/In5_MatrixDouble']);
        set_param([model_name '/In5_MatrixDouble'], 'OutDataTypeStr', 'double');
        if hasTunableParams == true
            set_param([model_name '/In5_MatrixDouble'],  'Value',          'matrixConstant2');
        else
            set_param([model_name '/In5_MatrixDouble'], 'Value',          'rand(6,6)');
        end
        
        add_block('simulink/Sources/Constant', [model_name '/In6_MatrixUint32']);
        set_param([model_name '/In6_MatrixUint32'],  'OutDataTypeStr', 'uint32');
        if hasStructParams == true
            set_param([model_name '/In6_MatrixUint32'], 'Value',          'structMixed.mat');
        else
            set_param([model_name '/In6_MatrixUint32'],  'Value',          'ones(6,6)');
        end
    end
    
    if (modelComplexity >= 4)
        add_block('simulink/Sources/Constant', [model_name '/In7_3DMatrixDouble']);
        set_param([model_name '/In7_3DMatrixDouble'], 'OutDataTypeStr', 'double');
        if hasTunableParams == true
            set_param([model_name '/In7_3DMatrixDouble'],  'Value',          'matrixConstant3d');
        else
            set_param([model_name '/In7_3DMatrixDouble'], 'Value',          'rand(4,4,4)');
        end
        
        add_block('simulink/Sources/Constant', [model_name '/In8_3DMatrixUint32']);
        set_param([model_name '/In8_3DMatrixUint32'],  'OutDataTypeStr', 'uint32');
        if hasStructParams == true
            set_param([model_name '/In8_3DMatrixUint32'], 'Value',          'structMixed.mat3d');
        else
            set_param([model_name '/In8_3DMatrixUint32'],  'Value',          'ones(4,4,4)');
        end
    end
end

% output ports
if hasOutputs == true
    add_block('simulink/Sinks/Out1',  [model_name '/Out1_ScalarDouble']);
    set_param([model_name '/Out1_ScalarDouble'], 'IconDisplay',    'Signal name');
    set_param([model_name '/Out1_ScalarDouble'], 'OutDataTypeStr', 'double');
    
    add_block('simulink/Sinks/Out1',  [model_name '/Out2_ScalarUint32']);
    set_param([model_name '/Out2_ScalarUint32'], 'IconDisplay',    'Signal name');
    set_param([model_name '/Out2_ScalarUint32'], 'OutDataTypeStr', 'uint32');
    
    if modelComplexity >= 2
        add_block('simulink/Sinks/Out1',  [model_name '/Out3_VectorDouble']);
        set_param([model_name '/Out3_VectorDouble'], 'IconDisplay',    'Signal name');
        set_param([model_name '/Out3_VectorDouble'], 'OutDataTypeStr', 'double');
        
        add_block('simulink/Sinks/Out1',  [model_name '/Out4_VectorUint32']);
        set_param([model_name '/Out4_VectorUint32'], 'IconDisplay',    'Signal name');
        set_param([model_name '/Out4_VectorUint32'], 'OutDataTypeStr', 'uint32');
    end
    
    if modelComplexity >= 3
        add_block('simulink/Sinks/Out1',  [model_name '/Out5_MatrixDouble']);
        set_param([model_name '/Out5_MatrixDouble'], 'IconDisplay',    'Signal name');
        set_param([model_name '/Out5_MatrixDouble'], 'OutDataTypeStr', 'double');
        
        add_block('simulink/Sinks/Out1',  [model_name '/Out6_MatrixUint32']);
        set_param([model_name '/Out6_MatrixUint32'], 'IconDisplay',    'Signal name');
        set_param([model_name '/Out6_MatrixUint32'], 'OutDataTypeStr', 'uint32');
    end
    
    if modelComplexity >= 4
        % MARTe does not support 3D signals, so the ports output only a page of the 3D signal
        add_block('simulink/Sinks/Out1',  [model_name '/Out7_3DMatrixDouble']);
        set_param([model_name '/Out5_MatrixDouble'], 'IconDisplay',    'Signal name');
        set_param([model_name '/Out5_MatrixDouble'], 'OutDataTypeStr', 'double');
        
        % selector to select the output page
        add_block('simulink/Signal Routing/Selector', [model_name '/SelectorDouble'],...
            'NumberOfDimensions', '3', 'IndexOptions', 'Index vector (dialog),Index vector (dialog),Index vector (dialog)',...
            'Indices',           '[1:4],[1:4],2');
        
        add_block('simulink/Sinks/Out1',  [model_name '/Out8_3DMatrixUint32']);
        set_param([model_name '/Out6_MatrixUint32'], 'IconDisplay',    'Signal name');
        set_param([model_name '/Out6_MatrixUint32'], 'OutDataTypeStr', 'uint32');
        
        add_block('simulink/Signal Routing/Selector', [model_name '/Selector3DUint32'],...
            'NumberOfDimensions', '3', 'IndexOptions', 'Index vector (dialog),Index vector (dialog),Index vector (dialog)',...
            'Indices',           '[1:4],[1:4],2');
    end

else
    add_block('simulink/Sinks/Terminator', [model_name '/Out1_ScalarDouble']);
    add_block('simulink/Sinks/Terminator', [model_name '/Out2_ScalarUint32']);
    if modelComplexity >= 2
        add_block('simulink/Sinks/Terminator',  [model_name '/Out3_VectorDouble']);
        add_block('simulink/Sinks/Terminator',  [model_name '/Out4_VectorUint32']);
        if modelComplexity >= 3
           add_block('simulink/Sinks/Terminator',  [model_name '/Out5_MatrixDouble']);
           add_block('simulink/Sinks/Terminator',  [model_name '/Out6_MatrixUint32']);
        end
        if modelComplexity >= 4
           add_block('simulink/Sinks/Terminator',  [model_name '/Out7_3DMatrixDouble']);
           add_block('simulink/Sinks/Terminator',  [model_name '/Out8_3DMatrixUint32']);
        end
    end
end

if hasStructSignals == true
    add_block('simulink/Sinks/Out1',  [model_name '/Out20_NonVirtualBus']);
    set_param([model_name '/Out20_NonVirtualBus'], 'IconDisplay',    'Signal name');
    set_param([model_name '/Out20_NonVirtualBus'], 'OutDataTypeStr', 'Inherit: auto');
    
    % custom type for the nonvirtual bus
    evalin('base', 'clear bus1Elems;');
    evalin('base', 'bus1Elems(1) = Simulink.BusElement;');
    evalin('base', 'bus1Elems(1).Name = ''Signal1'';');
    evalin('base', 'bus1Elems(1).Dimensions = 1;');
    evalin('base', 'bus1Elems(1).DimensionsMode = ''Fixed'';');
    evalin('base', 'bus1Elems(1).DataType = ''uint32'';');
    evalin('base', 'bus1Elems(1).SampleTime = -1;');
    evalin('base', 'bus1Elems(1).Complexity = ''real'';');

    evalin('base', 'bus1Elems(2) = Simulink.BusElement;');
    evalin('base', 'bus1Elems(2).Name = ''Signal2'';');
    evalin('base', 'bus1Elems(2).Dimensions = 1;');
    evalin('base', 'bus1Elems(2).DimensionsMode = ''Fixed'';');
    evalin('base', 'bus1Elems(2).DataType = ''double'';');
    evalin('base', 'bus1Elems(2).SampleTime = -1;');
    evalin('base', 'bus1Elems(2).Complexity = ''real'';');

    evalin('base', 'STRUCTSIGNAL1 = Simulink.Bus;');
    evalin('base', 'STRUCTSIGNAL1.Elements = bus1Elems;');
    
    add_block('simulink/Signal Routing/Bus Creator', [model_name '/BusCreator1']);
    set_param([model_name '/BusCreator1'],         'Inputs',         '2');
    set_param([model_name '/BusCreator1'],         'NonVirtualBus',  'on');
    set_param([model_name '/BusCreator1'],         'OutDataTypeStr', 'Bus: STRUCTSIGNAL1');
    
    if modelComplexity > 1
        add_block('simulink/Sinks/Out1',  [model_name '/Out21_NonVirtualBus']);
        set_param([model_name '/Out21_NonVirtualBus'], 'IconDisplay',    'Signal name');
        set_param([model_name '/Out21_NonVirtualBus'], 'OutDataTypeStr', 'Inherit: auto');
        
        evalin('base', 'clear bus2Elems;');
        
        evalin('base', 'bus2Elems(1) = Simulink.BusElement;');
        evalin('base', 'bus2Elems(1).Name = ''Signal1'';');
        evalin('base', 'bus2Elems(1).Dimensions = 1;');
        evalin('base', 'bus2Elems(1).DimensionsMode = ''Fixed'';');
        evalin('base', 'bus2Elems(1).DataType = ''uint32'';');
        evalin('base', 'bus2Elems(1).SampleTime = -1;');
        evalin('base', 'bus2Elems(1).Complexity = ''real'';');

        evalin('base', 'bus2Elems(2) = Simulink.BusElement;');
        evalin('base', 'bus2Elems(2).Name = ''Signal2'';');
        evalin('base', 'bus2Elems(2).Dimensions = 1;');
        evalin('base', 'bus2Elems(2).DimensionsMode = ''Fixed'';');
        evalin('base', 'bus2Elems(2).DataType = ''STRUCTSIGNAL1'';');
        evalin('base', 'bus2Elems(2).SampleTime = -1;');
        evalin('base', 'bus2Elems(2).Complexity = ''real'';');

        evalin('base', 'STRUCTSIGNAL2 = Simulink.Bus;');
        evalin('base', 'STRUCTSIGNAL2.Elements = bus2Elems;');

        add_block('simulink/Signal Routing/Bus Creator', [model_name '/BusCreator2']);
        set_param([model_name '/BusCreator2'],         'Inputs',         '2');
        set_param([model_name '/BusCreator2'],         'NonVirtualBus',  'on');
        set_param([model_name '/BusCreator2'],         'OutDataTypeStr', 'Bus: STRUCTSIGNAL2');
    end
    
end

%% set block properties

gain1Param = '1';
gain2Param = '2';

if hasStructParams == false && hasStructArrayParams == false && hasTunableParams == false
    gain1Param = '1';
    gain2Param = '1';
elseif hasStructParams == false && hasStructArrayParams == false && hasTunableParams == true
    gain1Param = '1';
    gain2Param = 'vectorConstant';
elseif hasStructParams == true && hasStructArrayParams == false && hasTunableParams == true
	gain1Param = 'structScalar.nested1.one';
    gain2Param = 'structScalar.nested2.two';
elseif hasStructParams == false && hasStructArrayParams == true && hasTunableParams == true
	gain1Param = 'structParamArray(1).one';
    gain2Param = '1';
elseif hasStructParams == true && hasStructArrayParams == true && hasTunableParams == true
    gain1Param = 'structScalar.one';
    gain2Param = 'structMixed.structParamArray(1).one';
end

% math blocks
set_param([model_name '/Gain1'], 'Gain',           gain1Param);
set_param([model_name '/Gain1'], 'OutDataTypeStr', 'double');

set_param([model_name '/Gain2'], 'Gain',           gain2Param);
set_param([model_name '/Gain2'], 'OutDataTypeStr', 'uint32');

% change types if requested
if useType == 1
    
    if modelComplexity >= 3
        set_param([model_name '/In5_MatrixDouble'],   'OutDataTypeStr', 'uint8');
        set_param([model_name '/In6_MatrixUint32'],   'OutDataTypeStr', 'uint16');
        evalin('base', 'matrixConstant2 = uint8(ones(6,6));');
        evalin('base', 'structMixed.mat = uint16(ones(6, 6)*2);');
        if hasOutputs == true
            set_param([model_name '/Out5_MatrixDouble'],   'OutDataTypeStr', 'uint8');
            set_param([model_name '/Out6_MatrixUint32'],   'OutDataTypeStr', 'uint16');
        end
    end
    if modelComplexity >= 4
        set_param([model_name '/In7_3DMatrixDouble'], 'OutDataTypeStr', 'uint32');
        %set_param([model_name '/In8_3DMatrixUint32'], 'OutDataTypeStr', 'uint64'); % uint64 not yet supported by coder
        if hasOutputs == true
            set_param([model_name '/Out7_3DMatrixDouble'], 'OutDataTypeStr', 'uint32');
            %set_param([model_name '/Out8_3DMatrixUint32'], 'OutDataTypeStr', 'uint64'); % uint64 not yet supported by coder
        end
    end
    
end

if useType == 2
    
    if modelComplexity >= 3
        set_param([model_name '/In5_MatrixDouble'],   'OutDataTypeStr', 'int8');
        set_param([model_name '/In6_MatrixUint32'],   'OutDataTypeStr', 'int16');
        evalin('base', 'matrixConstant2 = int8(ones(6,6));');
        evalin('base', 'structMixed.mat = int16(ones(6, 6)*2);');
        if hasOutputs == true
            set_param([model_name '/Out5_MatrixDouble'],   'OutDataTypeStr', 'int8');
            set_param([model_name '/Out6_MatrixUint32'],   'OutDataTypeStr', 'int16');
        end
    end
    if modelComplexity >= 4
        set_param([model_name '/In7_3DMatrixDouble'], 'OutDataTypeStr', 'int32');
        %set_param([model_name '/In8_3DMatrixUint32'], 'OutDataTypeStr', 'int64'); % int64 not yet supported by coder
        if hasOutputs == true
            set_param([model_name '/Out7_3DMatrixDouble'], 'OutDataTypeStr', 'int32');
            %set_param([model_name '/Out8_3DMatrixUint32'], 'OutDataTypeStr', 'int64'); % int64 not yet supported by coder
        end
    end
    
end

if useType == 4
    
    if modelComplexity >= 3
        set_param([model_name '/In5_MatrixDouble'],   'OutDataTypeStr', 'int32');
        set_param([model_name '/In6_MatrixUint32'],   'OutDataTypeStr', 'single');
        evalin('base', 'matrixConstant2 = int32(ones(6,6));');
        evalin('base', 'structMixed.mat = single(ones(6, 6)*2);');
        if hasOutputs == true
            set_param([model_name '/Out5_MatrixDouble'],   'OutDataTypeStr', 'int32');
            set_param([model_name '/Out6_MatrixUint32'],   'OutDataTypeStr', 'single');
        end
    end
    if modelComplexity >= 4
        set_param([model_name '/In7_3DMatrixDouble'], 'OutDataTypeStr', 'int32');
        set_param([model_name '/In8_3DMatrixUint32'], 'OutDataTypeStr', 'single');
        if hasOutputs == true
            set_param([model_name '/Out7_3DMatrixDouble'], 'OutDataTypeStr', 'int32');
            set_param([model_name '/Out8_3DMatrixUint32'], 'OutDataTypeStr', 'single');
        end
    end
    
end

%% manage connections

add_line(model_name, 'In1_ScalarDouble/1', 'Gain1/1');
add_line(model_name, 'Gain1/1',            'Out1_ScalarDouble/1');
add_line(model_name, 'In2_ScalarUint32/1', 'Gain2/1');
add_line(model_name, 'Gain2/1',            'Out2_ScalarUint32/1');

if hasStructSignals == true
    add_line(model_name, 'Gain1/1',            'BusCreator1/2');
    add_line(model_name, 'Gain2/1',            'BusCreator1/1');
    add_line(model_name, 'BusCreator1/1',      'Out20_NonVirtualBus/1');
    
    if modelComplexity >= 2
        add_line(model_name, 'Gain2/1',            'BusCreator2/1');
        add_line(model_name, 'BusCreator2/1',      'Out21_NonVirtualBus/1');
        add_line(model_name, 'BusCreator1/1',      'BusCreator2/2');
    end
end

if modelComplexity >= 2
    add_line(model_name, 'In3_VectorDouble/1', 'Gain3/1')
    add_line(model_name, 'Gain3/1',            'Out3_VectorDouble/1');
    add_line(model_name, 'In4_VectorUint32/1', 'Gain4/1')
    add_line(model_name, 'Gain4/1',            'Out4_VectorUint32/1');
end

if modelComplexity >= 3
    add_line(model_name, 'In5_MatrixDouble/1', 'Gain5/1')
    add_line(model_name, 'Gain5/1',            'Out5_MatrixDouble/1');
    add_line(model_name, 'In6_MatrixUint32/1', 'Gain6/1')
    add_line(model_name, 'Gain6/1',            'Out6_MatrixUint32/1');
end

if modelComplexity >= 4
    if hasInputs == true
        add_line(model_name, 'In7_3DMatrixDouble/1', 'Concatenate7/1');
        add_line(model_name, 'In7_3DMatrixDouble/1', 'Concatenate7/2');
        add_line(model_name, 'In8_3DMatrixUint32/1', 'Concatenate8/1');
        add_line(model_name, 'In8_3DMatrixUint32/1', 'Concatenate8/2');
        
        add_line(model_name, 'Concatenate7/1', 'SelectorDouble/1');
        add_line(model_name, 'Concatenate8/1', 'Selector3DUint32/1');
    else
        add_line(model_name, 'In7_3DMatrixDouble/1', 'SelectorDouble/1');
        add_line(model_name, 'In8_3DMatrixUint32/1', 'Selector3DUint32/1');
    end

    add_line(model_name, 'SelectorDouble/1', 'Out7_3DMatrixDouble/1');
    add_line(model_name, 'Selector3DUint32/1', 'Out8_3DMatrixUint32/1');
end

%% signal naming

% name the signals
name_input_signal([model_name '/Out1_ScalarDouble'], 1, 'Out1_ScalarDouble');
name_input_signal([model_name '/Out2_ScalarUint32'], 1, 'Out2_ScalarUint32');

name_output_signal([model_name '/In1_ScalarDouble'], 1, 'In1_ScalarDouble');
name_output_signal([model_name '/In2_ScalarUint32'], 1, 'In2_ScalarUint32');

if hasStructSignals == true
    name_input_signal([model_name '/Out20_NonVirtualBus'], 1, 'Out20_NonVirtualBus');
    
    if modelComplexity >= 2
        name_input_signal([model_name '/Out21_NonVirtualBus'], 1, 'Out21_NonVirtualBus');
    end
    
end

if modelComplexity >= 2
    name_output_signal([model_name '/In3_VectorDouble'], 1, 'In3_VectorDouble');
    name_output_signal([model_name '/In4_VectorUint32'], 1, 'In4_VectorUint32');
    
    name_input_signal([model_name '/Out3_VectorDouble'], 1, 'Out3_VectorDouble');
    name_input_signal([model_name '/Out4_VectorUint32'], 1, 'Out4_VectorUint32');
end

if modelComplexity >= 3
    name_output_signal([model_name '/In5_MatrixDouble'], 1, 'In5_MatrixDouble');
    name_output_signal([model_name '/In6_MatrixUint32'], 1, 'In6_MatrixUint32');
    
    name_input_signal([model_name '/Out5_MatrixDouble'], 1, 'Out5_MatrixDouble');
    name_input_signal([model_name '/Out6_MatrixUint32'], 1, 'Out6_MatrixUint32');
end

if modelComplexity >= 4
    name_output_signal([model_name '/In7_3DMatrixDouble'], 1, 'In7_3DMatrixDouble');
    name_output_signal([model_name '/In8_3DMatrixUint32'], 1, 'In8_3DMatrixUint32');
    
    name_input_signal([model_name '/Out7_3DMatrixDouble'], 1, 'Out7_3DMatrixDouble');
    name_input_signal([model_name '/Out8_3DMatrixUint32'], 1, 'Out8_3DMatrixUint32');
end
%% arranging block layout
% alternatively to setting the position of each block, the system can be
% arranged automatically
Simulink.BlockDiagram.arrangeSystem(model_name);

%% code generation
% name of each option is available by right-clicking on the option name
% in Model Settings dialog and then on "What's This?"

% Solver
set_param(model_name, 'SolverType', 'Fixed-step');

% Code Generation
set_param(model_name, 'SystemTargetFile', 'ert_shrlib.tlc');
set_param(model_name, 'RTWVerbose', 0);

% Optimization
if hasTunableParams == true
    set_param(model_name, 'DefaultParameterBehavior', 'Tunable');
else
    set_param(model_name, 'DefaultParameterBehavior', 'Inlined');
end
set_param(model_name, 'OptimizationCustomize', 1);
set_param(model_name, 'GlobalVariableUsage', 'None');

% Report
set_param(model_name, 'GenerateReport', 0);

% Comments
set_param(model_name, 'GenerateComments', 0);

% Custom code
if hasGetmmiFcn == true
    set_param(model_name, 'CustomSourceCode', ...
        [ ...
        '#define CONCAT(str1, str2, str3) CONCAT_(str1, str2, str3)'            newline, ...
        '#define CONCAT_(str1, str2, str3) str1 ## str2 ## str3'                newline, ...
        '#define GET_MMI_FUNC    CONCAT(THIS_MODEL_NAME, _GetCAPImmi    ,   )'  newline, ...
        '#define RT_MODEL_STRUCT CONCAT(RT_MODEL_      , THIS_MODEL_NAME, _T)', newline, ...
                                                                                newline, ...
        'void* GET_MMI_FUNC(void* voidPtrToRealTimeStructure)'                  newline, ...
        '{'                                                                     newline, ...
        '   rtwCAPI_ModelMappingInfo* mmiPtr = &(rtmGetDataMapInfo( ( RT_MODEL_STRUCT * )(voidPtrToRealTimeStructure) ).mmi);' newline, ...
        '   return (void*) mmiPtr;'                                             newline, ...
        '}' ...
        ] ...
    );

    set_param(model_name, 'CustomDefine', '-DTHIS_MODEL_NAME=$(PRODUCT_NAME)');
end

% Interface
set_param(model_name, 'SupportComplex', 0);
set_param(model_name, 'SupportAbsoluteTime', 0);
set_param(model_name, 'SuppressErrorStatus', 1);

set_param(model_name, 'CodeInterfacePackaging', 'Reusable function');

set_param(model_name, 'RootIOFormat', 'Part of model data structure');

set_param(model_name, 'RTWCAPIParams', 1);
set_param(model_name, 'RTWCAPIRootIO', 1);

if hasAllocFcn == true
    set_param(model_name, 'GenerateAllocFcn', 1);
end

set_param(model_name, 'IncludeMdlTerminateFcn', 0);
set_param(model_name, 'CombineSignalStateStructs', 1);

set_param(model_name, 'ArrayLayout', dataOrientation);

% Templates
set_param(model_name, 'GenerateSampleERTMain', 0);

%% model build

try
    rtwbuild(model_name)
    model_compiled = true;
catch
    model_compiled = false;
end

    
%% save and close
save_system(model_name);
close_system(model_name);

% clean build directory
rmdir('slprj', 's');
rmdir([model_name '_ert_shrlib_rtw'], 's');
%delete(sprintf('%s.slx',model_name));
delete(sprintf('%s.slxc',model_name));
delete(sprintf('%s.slx.bak',model_name));

% 
warning('on','MATLAB:DELETE:FileNotFound');
warning('on', 'all');

end   % function

function name_input_signal(address, signal_index, signal_name)
    
    p = get_param(address, 'PortHandles');
    l = get_param(p.Inport(signal_index),'Line');
    set_param(l,'Name', signal_name);
    
end

function name_output_signal(address, signal_index, signal_name)
    
    p = get_param(address, 'PortHandles');
    l = get_param(p.Outport(signal_index),'Line');
    set_param(l,'Name', signal_name);
    
end