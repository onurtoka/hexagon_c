# Hexagonal Architecture Documentation

## Overview

This project demonstrates a **Hexagonal Architecture** (also known as Ports and Adapters) implementation in C++17. The architecture emphasizes separation of concerns, testability, and maintainability by isolating the core business logic from external concerns.

## Architecture Layers

The system is organized into distinct layers with clear boundaries:

### Color Legend
- **Domain/Core**: #2e7d32 (green) - Business logic, entities, value objects
- **Ports**: #1565c0 (blue) - Interfaces defining contracts
- **Adapters**: #ef6c00 (orange) - Implementation of external integrations  
- **Infrastructure**: #616161 (gray) - Framework and technical concerns
- **External Systems**: #6a1b9a (purple) - Third-party services and databases

## High-Level Component Diagram

<div class="diagram-container">
<div class="diagram-title">Hexagonal Architecture Overview</div>
<div class="color-legend">
    <div class="legend-item"><div class="legend-box legend-domain"></div>Domain/Core</div>
    <div class="legend-item"><div class="legend-box legend-port"></div>Ports</div>
    <div class="legend-item"><div class="legend-box legend-adapter"></div>Adapters</div>
    <div class="legend-item"><div class="legend-box legend-infrastructure"></div>Infrastructure</div>
    <div class="legend-item"><div class="legend-box legend-external"></div>External Systems</div>
</div>

```mermaid
graph TB
    %% External Systems
    HttpReq[HTTP Request]:::hexagon-external
    ZMQBroker[ZeroMQ Broker]:::hexagon-external
    FileSystem[File System]:::hexagon-external
    Database[(Database)]:::hexagon-external
    
    %% Incoming Adapters
    HttpAdapter[HTTP Controller]:::hexagon-adapter
    CLIAdapter[CLI Command]:::hexagon-adapter
    ZMQReceiver[ZMQ Data Receiver]:::hexagon-adapter
    
    %% Incoming Ports
    TrackPort[Track Processing Port]:::hexagon-port
    DataPort[Data Ingestion Port]:::hexagon-port
    
    %% Domain Core
    subgraph Core["🏗️ Domain Core"]
        TrackEntity[Track Entity]:::hexagon-domain
        DelayCalc[Delay Calculator]:::hexagon-domain
        TrackService[Track Service]:::hexagon-domain
        ProcessUseCase[Process Track Use Case]:::hexagon-domain
    end
    
    %% Outgoing Ports
    RepoPort[Repository Port]:::hexagon-port
    MessagePort[Messaging Port]:::hexagon-port
    
    %% Outgoing Adapters
    SqlRepo[SQL Repository]:::hexagon-adapter
    ZMQSender[ZMQ Data Sender]:::hexagon-adapter
    FileRepo[File Repository]:::hexagon-adapter
    
    %% Infrastructure
    HttpServer[HTTP Server]:::hexagon-infrastructure
    ZMQContext[ZMQ Context]:::hexagon-infrastructure
    DBConnection[DB Connection]:::hexagon-infrastructure
    
    %% Connections - External to Adapters
    HttpReq --> HttpAdapter
    ZMQBroker --> ZMQReceiver
    
    %% Connections - Adapters to Ports
    HttpAdapter --> TrackPort
    CLIAdapter --> TrackPort
    ZMQReceiver --> DataPort
    
    %% Connections - Ports to Core
    TrackPort --> ProcessUseCase
    DataPort --> TrackService
    
    %% Connections - Core Internal
    ProcessUseCase --> TrackService
    TrackService --> DelayCalc
    TrackService --> TrackEntity
    
    %% Connections - Core to Outgoing Ports
    TrackService --> RepoPort
    ProcessUseCase --> MessagePort
    
    %% Connections - Outgoing Ports to Adapters
    RepoPort --> SqlRepo
    RepoPort --> FileRepo
    MessagePort --> ZMQSender
    
    %% Connections - Adapters to External
    SqlRepo --> Database
    FileRepo --> FileSystem
    ZMQSender --> ZMQBroker
    
    %% Infrastructure connections
    HttpAdapter -.-> HttpServer
    ZMQReceiver -.-> ZMQContext
    ZMQSender -.-> ZMQContext
    SqlRepo -.-> DBConnection

    classDef hexagon-domain fill:#2e7d32,stroke:#1b5e20,stroke-width:2px,color:#fff
    classDef hexagon-port fill:#1565c0,stroke:#0d47a1,stroke-width:2px,color:#fff
    classDef hexagon-adapter fill:#ef6c00,stroke:#e65100,stroke-width:2px,color:#fff
    classDef hexagon-infrastructure fill:#616161,stroke:#424242,stroke-width:2px,color:#fff
    classDef hexagon-external fill:#6a1b9a,stroke:#4a148c,stroke-width:2px,color:#fff
```
</div>

## End-to-End Data Flow

<div class="diagram-container">
<div class="diagram-title">Track Processing Flow</div>

```mermaid
flowchart TD
    Start([Start: Track Data Received]):::hexagon-external
    
    %% Input Processing
    Receive[ZMQ Data Receiver<br/>Parse JSON]:::hexagon-adapter
    Validate[Validate Track Data<br/>Check Format]:::hexagon-adapter
    
    %% Core Processing
    UseCase[Process Track Use Case<br/>Orchestrate Workflow]:::hexagon-domain
    CalcDelay[Calculate Delay<br/>Business Logic]:::hexagon-domain
    CreateResult[Create Calculated Track<br/>Domain Entity]:::hexagon-domain
    
    %% Output Processing
    Store[Store in Repository<br/>Persistence]:::hexagon-adapter
    Publish[Publish via ZMQ<br/>Send Results]:::hexagon-adapter
    
    End([End: Results Distributed]):::hexagon-external
    
    %% Flow connections
    Start --> Receive
    Receive --> Validate
    Validate -->|Valid Data| UseCase
    Validate -->|Invalid| Error[Log Error]:::hexagon-infrastructure
    
    UseCase --> CalcDelay
    CalcDelay --> CreateResult
    CreateResult --> Store
    CreateResult --> Publish
    
    Store --> End
    Publish --> End
    Error --> End
    
    %% Decision styling
    Validate -.->|Validation Rules| Domain[Domain Rules]:::hexagon-domain
    
    classDef hexagon-domain fill:#2e7d32,stroke:#1b5e20,stroke-width:2px,color:#fff
    classDef hexagon-adapter fill:#ef6c00,stroke:#e65100,stroke-width:2px,color:#fff
    classDef hexagon-infrastructure fill:#616161,stroke:#424242,stroke-width:2px,color:#fff
    classDef hexagon-external fill:#6a1b9a,stroke:#4a148c,stroke-width:2px,color:#fff
```
</div>

## Sequence Diagram: Track Processing Use Case

<div class="diagram-container">
<div class="diagram-title">Track Processing Sequence</div>

```mermaid
sequenceDiagram
    participant Ext as External System
    participant ZMQRec as ZMQ Receiver
    participant Port as Incoming Port
    participant UseCase as Process Use Case
    participant Service as Track Service
    participant Calc as Delay Calculator
    participant Repo as Repository
    participant ZMQSend as ZMQ Sender
    participant Broker as ZMQ Broker
    
    Ext->>+ZMQRec: Send Track Data (JSON)
    Note over ZMQRec: Adapter Layer
    ZMQRec->>ZMQRec: Parse & Validate JSON
    ZMQRec->>+Port: processTrackData(trackData)
    Note over Port: Port Layer
    
    Port->>+UseCase: execute(trackData)
    Note over UseCase,Calc: Domain Core
    UseCase->>+Service: processTrack(trackData)
    Service->>+Calc: calculateDelay(track)
    Calc-->>-Service: delayResult
    Service->>Service: createCalculatedTrack()
    Service-->>-UseCase: calculatedTrack
    
    UseCase->>+Repo: save(calculatedTrack)
    Note over Repo: Adapter Layer
    Repo-->>-UseCase: success
    
    UseCase->>+ZMQSend: publish(calculatedTrack)
    ZMQSend->>ZMQSend: serialize to JSON
    ZMQSend->>+Broker: send message
    Broker-->>-ZMQSend: ack
    ZMQSend-->>-UseCase: success
    
    UseCase-->>-Port: result
    Port-->>-ZMQRec: success
    ZMQRec-->>-Ext: processing complete
    
    Note over Ext,Broker: Colors: External(purple), Adapters(orange), Ports(blue), Domain(green)
```
</div>

## Component Responsibilities

### Domain Core (Green)
- **Entities**: `TrackData`, `DelayCalculatedTrackData` - Core business objects
- **Value Objects**: Immutable data structures with business meaning
- **Domain Services**: `CalculatorService` - Business logic implementation
- **Use Cases**: High-level business workflows and orchestration

### Ports (Blue)
- **Incoming Ports**: Interfaces for external actors to interact with the system
- **Outgoing Ports**: Interfaces for the system to interact with external resources
- Define contracts without implementation details

### Adapters (Orange)
- **Incoming Adapters**: `ZeroMQDataReceiver` - Convert external requests to domain calls
- **Outgoing Adapters**: `ZeroMQDataSender` - Convert domain calls to external actions
- Handle serialization, protocol specifics, and external API integration

### Infrastructure (Gray)
- **Frameworks**: ZeroMQ context management, HTTP servers
- **Cross-cutting Concerns**: Logging, monitoring, configuration
- **Technical Implementation**: Connection pooling, threading

### External Systems (Purple)
- **Message Brokers**: ZeroMQ broker infrastructure
- **Databases**: Persistent storage systems
- **APIs**: Third-party service integrations
- **File Systems**: Local or distributed storage

## Benefits of This Architecture

1. **Testability**: Core logic can be tested without external dependencies
2. **Flexibility**: Easy to swap implementations (e.g., ZMQ → HTTP, SQL → NoSQL)
3. **Maintainability**: Clear separation of concerns reduces coupling
4. **Scalability**: Each layer can be optimized independently
5. **SOLID Principles**: Architecture naturally enforces good design principles

## Testing Strategy

- **Unit Tests**: Test domain logic in isolation
- **Integration Tests**: Test adapter implementations
- **Contract Tests**: Verify port interfaces
- **End-to-End Tests**: Full system workflow validation
